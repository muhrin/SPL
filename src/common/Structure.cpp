/*
 * Structure.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/common/Structure.h"

#include <vector>

#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>

extern "C" {
#  include <spglib/spglib.h>
}

#include "spl/SSLibAssert.h"
#include "spl/common/Atom.h"
#include "spl/common/AtomsFormula.h"
#include "spl/common/Types.h"
#include "spl/common/UnitCell.h"
#include "spl/utility/IndexingEnums.h"

#ifdef _MSC_VER
// Disable warning about passing this pointer to DistanceCalculator in initialisation list
#  pragma warning( push )
#  pragma warning( disable : 4355 )
#endif

namespace spl {
namespace common {

class MatchSpecies : public std::unary_function< const Atom &, bool>
{
public:
  MatchSpecies(const AtomSpeciesId::Value toMatch) :
      mySpecies(toMatch)
  {
  }

  bool
  operator()(const Atom & atom)
  {
    return atom.getSpecies() == mySpecies;
  }

private:
  const AtomSpeciesId::Value mySpecies;
};

Structure::Structure() :
    myAtomPositionsCurrent(false), myDistanceCalculator(*this)
{
}

Structure::Structure(const UnitCell & cell) :
    myAtomPositionsCurrent(false), myDistanceCalculator(*this)
{
  setUnitCell(cell);
}

Structure::Structure(const Structure & toCopy) :
    myAtomPositionsCurrent(false), myDistanceCalculator(*this)
{
  // Use the equals operator so we don't duplicate code
  *this = toCopy;
}

Structure::~Structure()
{
}

Structure &
Structure::operator =(const Structure & rhs)
{
  PropertiesObject::operator =(rhs);

  myName = rhs.myName;

  // Copy over the unit cell (if exists)
  if(rhs.myCell)
    setUnitCell(*rhs.myCell);
  else
    clearUnitCell();

  // Copy over the atoms
  clearAtoms();
  BOOST_FOREACH(const Atom & atom, rhs.myAtoms)
  {
    newAtom(atom);
  }

  return *this;
}

StructurePtr
Structure::clone() const
{
  return StructurePtr(new Structure(*this));
}

void
Structure::updateWith(const Structure & structure)
{
  // Update the unit cell
  if(structure.getUnitCell())
    setUnitCell(*structure.getUnitCell());
  else
    clearUnitCell();

  // Update the atoms
  clearAtoms();
  BOOST_FOREACH(const Atom & atom, structure.myAtoms)
  {
    newAtom(atom);
  }

  // Update the properties
  getProperties().insert(structure.getProperties(), true);
}

const std::string &
Structure::getName() const
{
  return myName;
}

void
Structure::setName(const std::string & name)
{
  myName = name;
}

UnitCell *
Structure::getUnitCell()
{
  return ::boost::get_pointer(myCell);
}

const UnitCell *
Structure::getUnitCell() const
{
  return ::boost::get_pointer(myCell);
}

void
Structure::setUnitCell(const UnitCell & cell)
{
  if(myCell && *myCell == cell)
    return;

  myCell.reset(cell);
  myDistanceCalculator.setUnitCell(::boost::get_pointer(myCell));
}

void
Structure::clearUnitCell()
{
  myCell.reset();
  myDistanceCalculator.setUnitCell(NULL);
}

size_t
Structure::getNumAtoms() const
{
  return myAtoms.size();
}

Structure::AtomIterator
Structure::atomsBegin()
{
  return myAtoms.begin();
}

Structure::AtomIterator
Structure::atomsEnd()
{
  return myAtoms.end();
}

Atom &
Structure::getAtom(const size_t idx)
{
  SSLIB_ASSERT(idx < getNumAtoms());

  return myAtoms[idx];
}

const Atom &
Structure::getAtom(const size_t idx) const
{
  SSLIB_ASSERT(idx < getNumAtoms());

  return myAtoms[idx];
}

Atom &
Structure::newAtom(const AtomSpeciesId::Value species)
{
  myAtomPositionsCurrent = false;
  Atom * const atom = new Atom(species, myAtoms.size());
  myAtoms.push_back(atom);
  atom->addListener(this);
  return *atom;
}

Atom &
Structure::newAtom(const Atom & toCopy)
{
  myAtomPositionsCurrent = false;
  Atom & atom = *myAtoms.insert(myAtoms.end(), new Atom(toCopy));
  atom.setIndex(myAtoms.size());
  atom.addListener(this);
  return atom;
}

Structure::AtomIterator
Structure::eraseAtom(AtomIterator & it)
{
  const size_t index = it->getIndex();
  it->removeListener(this);
  const AtomIterator ret = myAtoms.erase(it);

  for(size_t i = index; i < myAtoms.size(); ++i)
    myAtoms[i].setIndex(i);

  myAtomPositionsCurrent = false;
  return ret;
}

bool
Structure::removeAtom(const Atom & atom)
{
  const size_t index = atom.getIndex();
  if(index >= myAtoms.size() || &atom != &myAtoms[index])
    return false;

  AtomIterator it = myAtoms.begin() + index;
  eraseAtom(it);
  return true;
}

size_t
Structure::clearAtoms()
{
  const size_t previousNumAtoms = myAtoms.size();

  myAtoms.clear();

  myAtomPositionsCurrent = false;
  return previousNumAtoms;
}

void
Structure::getAtomPositions(::arma::mat & posMtx) const
{
  // Do we need to update the buffer?
  if(!myAtomPositionsCurrent)
    updatePosBuffer();

  posMtx = myAtomPositionsBuffer;
}

void
Structure::getAtomPositions(::arma::subview< double> & posMtx) const
{
  // Do we need to update the buffer?
  if(!myAtomPositionsCurrent)
    updatePosBuffer();

  posMtx = myAtomPositionsBuffer;
}

void
Structure::setAtomPositions(const ::arma::mat & posMtx)
{
  const size_t numAtoms = getNumAtoms();
  SSLIB_ASSERT(posMtx.n_rows == 3 && posMtx.n_cols == numAtoms);

  for(size_t i = 0; i < numAtoms; ++i)
  {
    myAtoms[i].setPosition(posMtx.col(i));
  }

  // Save the new positions in the buffer
  myAtomPositionsBuffer = posMtx;
  myAtomPositionsCurrent = true;
}

size_t
Structure::getNumAtomsOfSpecies(const AtomSpeciesId::Value species) const
{
  return ::std::count_if(myAtoms.begin(), myAtoms.end(), MatchSpecies(species));
}

AtomsFormula
Structure::getComposition() const
{
  AtomsFormula comp;
  BOOST_FOREACH(const Atom & atom, myAtoms)
  {
    comp += AtomsFormula(atom.getSpecies());
  }
  return comp;
}

const DistanceCalculator &
Structure::getDistanceCalculator() const
{
  return myDistanceCalculator;
}

::boost::optional< ::std::string>
Structure::getVisibleProperty(const VisibleProperty & property) const
{
  return property.getValue(getProperties());
}

void
Structure::setVisibleProperty(VisibleProperty & property,
    const ::std::string & value)
{
  property.setValue(getProperties(), value);
}

bool
Structure::makePrimitive()
{
  const size_t numAtoms = myAtoms.size();
  if(!getUnitCell() || numAtoms == 0)
    return false;

  if(numAtoms > 0 && myCell)
  {
    double lattice[3][3];
    const ::arma::mat33 & orthoMtx = myCell->getOrthoMtx();
    for(size_t i = 0; i < 3; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
      {
        // Row-major = column-major
        lattice[i][j] = orthoMtx(i, j);
      }
    }

    double (*positions)[3] = new double[numAtoms][3];
    ::arma::mat posMtx;
    getAtomPositions(posMtx);
    myCell->cartsToFracInplace(posMtx);
    for(size_t i = 0; i < numAtoms; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
      {
        // Row-major = column-major
        positions[i][j] = posMtx(j, i);
      }
    }

    ::std::vector< AtomSpeciesId::Value> speciesVec;
    ::std::vector< AtomSpeciesId::Value> speciesIdxVec;
    getAtomSpecies(::std::back_inserter(speciesVec));
    ::std::map< common::AtomSpeciesId::Value, int> speciesIndices;
    int idx = 0;
    BOOST_FOREACH(const common::AtomSpeciesId::Value & speciesId, speciesVec)
    {
      if(speciesIndices.insert(::std::make_pair(speciesId, idx)).second == true)
      {
        speciesIdxVec.push_back(speciesId);
        ++idx;
      }
    }

    ::boost::scoped_array< int> species(new int[speciesVec.size()]);
    for(size_t i = 0; i < speciesVec.size(); ++i)
    {
      species[i] = speciesIndices[speciesVec[i]];
    }

    // Try to find the primitive unit cell
    const size_t newNumAtoms = static_cast< size_t>(spg_find_primitive(lattice,
        positions, species.get(), numAtoms, 0.05));

    if(newNumAtoms != 0 && newNumAtoms < numAtoms)
    {
      // First deal with lattice
      ::arma::mat33 newLattice;
      for(size_t i = 0; i < 3; ++i)
      {
        for(size_t j = 0; j < 3; ++j)
        {
          newLattice(i, j) = lattice[i][j];
        }
      }

      myCell->setOrthoMtx(newLattice);

      // Now deal with atoms
      clearAtoms();

      Atom * atom;
      ::arma::vec3 pos;

      for(size_t i = 0; i < newNumAtoms; ++i)
      {
        atom = &newAtom(speciesIdxVec[species[i]]);
        pos << positions[i][0] << ::arma::endr << positions[i][1]
            << ::arma::endr << positions[i][2] << ::arma::endr;
        atom->setPosition(myCell->fracWrapToCartInplace(pos));
      }
      delete[] positions;
      return true;
    }
  }

  return false;
}

UniquePtr< Structure>::Type
Structure::getPrimitiveCopy() const
{
  UniquePtr< Structure>::Type structure(new Structure(*this));
  structure->makePrimitive();
  return structure;
}

void
Structure::scale(const double scaleFactor)
{
  UnitCell * const unitCell = getUnitCell();

  if(unitCell)
  {
    const double volume = unitCell->getVolume();
    ::arma::mat atomPositions;
    getAtomPositions(atomPositions);
    unitCell->cartsToFracInplace(atomPositions); // Generate fractional positions
    unitCell->setVolume(volume * scaleFactor); // Scale the unit cell
    setAtomPositions(unitCell->fracsToCartInplace(atomPositions)); // Use the scaled cell to convert back to cart
  }
  else
  {
    // TODO: Implement cluster scaling
  }
}

void
Structure::print(::std::ostream & os) const
{
  using namespace utility::cell_params_enum;

  os << "Structure";
  if(!myName.empty())
    os << " " << myName;
  os << ":" << std::endl;

  const UnitCell * const unitCell = getUnitCell();
  if(unitCell)
  {
    const double (&params)[6] = unitCell->getLatticeParams();
    os << "Unit cell: " << params[A] << " " << params[B] << " " << params[C]
        << params[ALPHA] << " " << params[BETA] << " " << params[GAMMA]
        << std::endl;
  }

  ::arma::vec3 pos;
  os << "Atoms" << std::endl;
  for(size_t i = 0; i < getNumAtoms(); ++i)
  {
    const Atom & atom = getAtom(i);
    // Species
    //os << atom.getSpecies().toString();
    // Positions
    pos = atom.getPosition();
    for(size_t i = 0; i < 3; ++i)
      os << " " << pos(i);
    os << std::endl;
  }
}

void
Structure::onAtomMoved(Atom * const atom)
{
  // Atom has moved so the buffer is not longer current
  myAtomPositionsCurrent = false;
}

void
Structure::onAtomDestroyed(Atom * const atom)
{
}

void
Structure::updatePosBuffer() const
{
  myAtomPositionsBuffer.reset();
  myAtomPositionsBuffer.set_size(3, getNumAtoms());
  for(size_t i = 0; i < getNumAtoms(); ++i)
  {
    myAtomPositionsBuffer.col(i) = myAtoms[i].getPosition();
  }
  myAtomPositionsCurrent = true;
}

} // namespace common
} // namespace spl

// Global namespace
std::ostream &
operator<<(std::ostream & os, const spl::common::Structure & structure)
{
  structure.print(os);
  return os;
}
