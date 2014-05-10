/*
 * Structure.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_H
#define STRUCTURE_H

// INCLUDES ///////////////////////////////////////////////
#include "spl/SSLib.h"

#include <map>
#include <memory>
#include <ostream>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <armadillo>

#include "spl/common/Atom.h"
#include "spl/common/AtomSpeciesId.h"
#include "spl/common/DistanceCalculatorDelegator.h"
#include "spl/common/StructureProperties.h"
#include "spl/common/Types.h"
#include "spl/common/UnitCell.h"
#include "spl/utility/PropertiesObject.h"
#include "spl/utility/NamedProperty.h"

std::ostream &
operator<<(std::ostream & os, const spl::common::Structure & p);

std::ostream &
operator<<(std::ostream & os, const spl::common::Structure & p);

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class AtomsFormula;
class DistanceCalculator;

class Structure : public utility::PropertiesObject, Atom::Listener
{
  typedef ::boost::ptr_vector< Atom> AtomsContainer;
public:
  typedef common::Atom StructureAtom;
  typedef AtomsContainer::iterator AtomIterator;
  typedef utility::NamedProperty< utility::HeterogeneousMap> VisibleProperty;

  explicit
  Structure();
  explicit
  Structure(const UnitCell & cell);
  Structure(const Structure & toCopy);
  virtual
  ~Structure();
  Structure &
  operator =(const Structure & rhs);
  StructurePtr
  clone() const;

  void
  updateWith(const Structure & structure);

  const std::string &
  getName() const;
  void
  setName(const std::string & name);

  // UNIT CELL /////////////////////////////////////////
  UnitCell *
  getUnitCell();
  const UnitCell *
  getUnitCell() const;

  // Set the unit cell to be used by the structure.
  void
  setUnitCell(const UnitCell & cell);
  void
  clearUnitCell();

  // ATOMS ///////////////////////////////////////////////
  size_t
  getNumAtoms() const;

  AtomIterator
  atomsBegin();
  AtomIterator
  atomsEnd();
  Atom &
  getAtom(const size_t idx);
  const Atom &
  getAtom(const size_t idx) const;

  Atom &
  newAtom(const AtomSpeciesId::Value species);
  Atom &
  newAtom(const Atom & toCopy);
  AtomIterator
  eraseAtom(AtomIterator & it);
  bool
  removeAtom(const Atom & atom);
  size_t
  clearAtoms();

  void
  getAtomPositions(::arma::mat & posMtx) const;
  void
  getAtomPositions(::arma::subview< double> & posMtx) const;
  void
  setAtomPositions(const ::arma::mat & posMtx);

  template< typename OutputIterator>
    void
    getAtomSpecies(OutputIterator it) const;
  size_t
  getNumAtomsOfSpecies(const AtomSpeciesId::Value species) const;
  AtomsFormula
  getComposition() const;

  const DistanceCalculator &
  getDistanceCalculator() const;

  ::boost::optional< ::std::string>
  getVisibleProperty(const VisibleProperty & property) const;
  void
  setVisibleProperty(VisibleProperty & property, const ::std::string & value);

  bool
  makePrimitive();

  UniquePtr< Structure>::Type
  getPrimitiveCopy() const;

  void
  scale(const double scaleFactor);

  void
  print(::std::ostream & os) const;

private:
  virtual void
  onAtomMoved(Atom * const atom);
  virtual void
  onAtomDestroyed(Atom * const atom);

  void
  updatePosBuffer() const;

  /** The name of this structure, set by calling code */
  std::string myName;

  /** The unit cell for this crystal structure. */
  ::boost::optional< UnitCell> myCell;

  /** The atoms contained in this group */
  AtomsContainer myAtoms;

  // Flag to indicate whether the structure has changed since
  // the last time that all atom positions were requested
  mutable bool myAtomPositionsCurrent;
  mutable ::arma::mat myAtomPositionsBuffer;

  mutable DistanceCalculatorDelegator myDistanceCalculator;

  friend class Atom;
};

template< typename OutputIterator>
  void
  Structure::getAtomSpecies(OutputIterator it) const
  {
    BOOST_FOREACH(const Atom & atom, myAtoms)
      *it = atom.getSpecies();
  }

}
}

#endif /* STRUCTURE_H */
