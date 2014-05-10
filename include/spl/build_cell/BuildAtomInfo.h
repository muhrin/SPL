/*
 * BuildAtomInfo.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef BUILD_ATOM_INFO_H
#define BUILD_ATOM_INFO_H

// INCLUDES ////////////
#include <vector>

#include <boost/shared_ptr.hpp>

#include "spl/OptionalTypes.h"

// DEFINITION ///////////////////////

namespace spl {
// FORWARD DECLARATIONS ///////
namespace common {
class Atom;
class AtomGroup;
}

namespace build_cell {
class StructureBuild;

class BuildAtomInfo
{
public:
  typedef ::boost::shared_ptr<common::AtomGroup> AtomGroupPtr;
  typedef ::std::vector<bool> OpMask;

  size_t getNumAtoms() const;
  common::Atom & getAtom(const size_t index);
  const common::Atom & getAtom(const size_t index) const;

  common::AtomGroup * getGroup();
  const common::AtomGroup * getGroup() const;

  void setFixed(const bool fixed);
  bool isFixed() const;

  int getMultiplicity() const;
  void setMultiplicity(const int multiplicity);

  const OpMask & getOpMask() const;
  void setOperatorsMask(const OpMask & opMask);

  OptionalDouble radius;

private:
  typedef ::std::vector<common::Atom *> AtomsList;

  BuildAtomInfo(common::Atom & atom);

  void addAtom(common::Atom & atom);
  void removeAtom(common::Atom & atom);

  AtomsList myAtoms;
  AtomGroupPtr myGroup; 
  bool myFixed;
  int myMultiplicity;
  OpMask myOpMask;

  friend class StructureBuild;
};

}
}


#endif /* BUILD_ATOM_INFO_H */
