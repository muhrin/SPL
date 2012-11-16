/*
 * Structure.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////////////////////////
#include "common/Structure.h"

#include <vector>

#include <boost/foreach.hpp>

extern "C"
{
#  include <spglib/spglib.h>
}

#include "SSLibAssert.h"
#include "common/Atom.h"
#include "common/Types.h"
#include "common/UnitCell.h"

#ifdef _MSC_VER
// Disable warning about passing this pointer to DistanceCalculator in initialisation list
#pragma warning( disable : 4355 )
#endif

namespace sstbx {
namespace common {

class MatchSpecies : public std::unary_function<const Atom &, bool>
{
public:
  MatchSpecies(const AtomSpeciesId::Value toMatch):
  mySpecies(toMatch) {}

  bool operator() (const Atom & atom) {return atom.getSpecies() == mySpecies;}

private:
  const AtomSpeciesId::Value mySpecies;
};

Structure::Structure(UnitCellPtr cell):
myCell(cell),
myAtomPositionsCurrent(false),
myNumAtoms(0),
myDistanceCalculator(*this)
{}

Structure::Structure(const Structure & toCopy):
myName(toCopy.myName),
myNumAtoms(toCopy.myNumAtoms),
myTypedProperties(toCopy.myTypedProperties),
myDistanceCalculator(*this)
{
  // Copy over the unit cell (if exists)
  if(toCopy.myCell.get())
    myCell = toCopy.myCell->clone();

  // Copy over the atoms
  BOOST_FOREACH(const Atom & atom, toCopy.myAtoms)
  {
    newAtom(atom);
  }
}

const std::string & Structure::getName() const
{
	return myName;
}

void Structure::setName(const std::string & name)
{
	myName = name;
}

UnitCell * Structure::getUnitCell()
{
	return myCell.get();
}

const UnitCell * Structure::getUnitCell() const
{
	return myCell.get();
}

void Structure::setUnitCell(UnitCellPtr cell)
{
  if(myCell.get() == cell.get())
    return;

  if(myCell.get())
    myCell->setStructure(NULL);

	myCell = cell;
  myCell->setStructure(this);
  myDistanceCalculator.unitCellChanged();
}

size_t Structure::getNumAtoms() const
{
  return myAtoms.size();
}

Atom & Structure::getAtom(const size_t idx)
{
  SSLIB_ASSERT(idx < getNumAtoms());

  return myAtoms[idx];
}

const Atom & Structure::getAtom(const size_t idx) const
{
  SSLIB_ASSERT(idx < getNumAtoms());

  return myAtoms[idx];
}

Atom & Structure::newAtom(const AtomSpeciesId::Value species)
{
  Atom * const atom = new Atom(species, *this, myNumAtoms++);
  myAtoms.push_back(atom);
  return *atom;
}

Atom & Structure::newAtom(const Atom & toCopy)
{
  return *myAtoms.insert(myAtoms.end(), new Atom(toCopy, *this, ++myNumAtoms));
}

bool Structure::removeAtom(const Atom & atom)
{
  if(&atom.getStructure() != this)
    return false;

  const size_t index = atom.getIndex();

  myAtoms.erase(myAtoms.begin() + index);
  --myNumAtoms;

  for(size_t i = index; i < myNumAtoms; ++i)
  {
    myAtoms[i].setIndex(i);
  }

  myAtomPositionsCurrent = false;
  return true;
}

size_t Structure::clearAtoms()
{
  const size_t previousNumAtoms = myNumAtoms;

  myAtoms.clear();

  myNumAtoms = 0;
  myAtomPositionsCurrent = false;
  return previousNumAtoms;
}

void Structure::getAtomPositions(::arma::mat & posMtx) const
{
	// Do we need to update the buffer?
	if(!myAtomPositionsCurrent)
	{
		myAtomPositionsBuffer.reset();
    myAtomPositionsBuffer.set_size(3, getNumAtoms());
		for(size_t i = 0; i < getNumAtoms(); ++i)
    {
      myAtomPositionsBuffer.col(i) = myAtoms[i].getPosition();
    }
		myAtomPositionsCurrent = true;
	}

	posMtx = myAtomPositionsBuffer;
}

void Structure::setAtomPositions(const ::arma::mat & posMtx)
{
  const size_t numAtoms = getNumAtoms();
  SSLIB_ASSERT(posMtx.n_rows == 3 && posMtx.n_cols == numAtoms);

  for(size_t i = 0; i < numAtoms; ++i)
  {
    myAtoms[i].setPosition(posMtx.col(i));
  }

	// Save the new positions in the buffer
	myAtomPositionsBuffer	= posMtx;
	myAtomPositionsCurrent	= true;
}

void Structure::getAtomSpecies(::std::vector<AtomSpeciesId::Value> & species) const
{
  const size_t numAtoms = getNumAtoms();
  species.resize(numAtoms);

  for(size_t i = 0; i < numAtoms; ++i)
  {
    species[i] = myAtoms[i].getSpecies();
  }
}

size_t Structure::getNumAtomsOfSpecies(const AtomSpeciesId::Value species) const
{
  return ::std::count_if(myAtoms.begin(), myAtoms.end(), MatchSpecies(species));
}

const DistanceCalculator & Structure::getDistanceCalculator() const
{
  return myDistanceCalculator;
}

bool Structure::makePrimitive()
{
  if(myNumAtoms > 0 && myCell.get())
  {
    double lattice[3][3];
    const::arma::mat33 & orthoMtx = myCell->getOrthoMtx();
    for(size_t i = 0; i < 3; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
      {
        // Row-major = column-major
        lattice[i][j] = orthoMtx(i, j);
      }
    }

    double (*positions)[3] = new double[myNumAtoms][3];
    ::arma::mat posMtx;
    getAtomPositions(posMtx);
    myCell->cartsToFracInplace(posMtx);
    for(size_t i = 0; i < myNumAtoms; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
      {
        // Row-major = column-major
        positions[i][j] = posMtx(j, i);
      }
    }

    ::std::vector<AtomSpeciesId::Value> speciesVec;
    getAtomSpecies(speciesVec);
    ::boost::scoped_array<int> species(new int[speciesVec.size()]);
    for(size_t i = 0; i < speciesVec.size(); ++i)
    {
      species[i] = speciesVec[i].ordinal();
    }

    // Try to find the primitive unit cell
    const size_t newNumAtoms = (size_t)spg_find_primitive(lattice, positions, species.get(), myNumAtoms, 0.05);

    if(newNumAtoms != 0 && newNumAtoms < myNumAtoms)
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
        atom = &newAtom(*AtomSpeciesId::values()[species[i]]);
        pos << positions[i][0] << ::arma::endr
          << positions[i][1] << ::arma::endr
          << positions[i][2] << ::arma::endr;
        atom->setPosition(myCell->wrapVecInplace(pos));
      }

      return true;
    }
  }

  return false;
}

UniquePtr<Structure>::Type Structure::getPrimitiveCopy() const
{
  UniquePtr<Structure>::Type structure;

  if(myNumAtoms > 0 && myCell.get())
  {
    // Get the lattice
    double lattice[3][3];
    const::arma::mat33 & orthoMtx = myCell->getOrthoMtx();
    for(size_t i = 0; i < 3; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
      {
        // Row-major = column-major
        lattice[i][j] = orthoMtx(i, j);
      }
    }

    // Get the atom positions
    double (*positions)[3] = new double[myNumAtoms][3];
    ::arma::mat posMtx;
    getAtomPositions(posMtx);
    myCell->cartsToFracInplace(posMtx);
    for(size_t i = 0; i < myNumAtoms; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
      {
        // Row-major = column-major
        positions[i][j] = posMtx(j, i);
      }
    }

    // Get the atom species
    ::std::vector<AtomSpeciesId::Value> speciesVec;
    getAtomSpecies(speciesVec);
    ::boost::scoped_array<int> species(new int[speciesVec.size()]);
    for(size_t i = 0; i < speciesVec.size(); ++i)
    {
      species[i] = speciesVec[i].ordinal();
    }

    // Try to find the primitive unit cell
    const size_t newNumAtoms = (size_t)spg_find_primitive(lattice, positions, species.get(), myNumAtoms, 0.05);

    if(newNumAtoms != 0 && newNumAtoms < myNumAtoms)
    {
      structure.reset(new Structure());

      // First deal with lattice
      ::arma::mat33 newLattice;
      for(size_t i = 0; i < 3; ++i)
      {
        for(size_t j = 0; j < 3; ++j)
        {
          newLattice(i, j) = lattice[i][j];
        }
      }

      structure->setUnitCell(UnitCellPtr(new UnitCell(newLattice)));
      const UnitCell * const unitCell = structure->getUnitCell();

      Atom * atom;
      ::arma::vec3 pos;
      for(size_t i = 0; i < newNumAtoms; ++i)
      {
        atom = &structure->newAtom(*AtomSpeciesId::values()[species[i]]);
        pos << positions[i][0] << ::arma::endr
          << positions[i][1] << ::arma::endr
          << positions[i][2] << ::arma::endr;
        atom->setPosition(unitCell->fracToCartInplace(unitCell->wrapVecFracInplace(pos)));
      }

    }
    delete [] positions;
  } // if(myCell)

  if(!structure.get())
    structure.reset(new Structure(*this));

  return structure;
}

void Structure::atomMoved(const Atom & atom) const
{
  // Atom has moved so the buffer is not longer current
  myAtomPositionsCurrent = false;
}

}
}
