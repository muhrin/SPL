/*
 * BuildAtomInfo.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/BuildAtomInfo.h"

#include "spl/SSLibAssert.h"
#include "spl/build_cell/StructureBuild.h"
#include "spl/common/Atom.h"

namespace spl {
namespace build_cell {

size_t BuildAtomInfo::getNumAtoms() const
{
  return myAtoms.size();
}

common::Atom & BuildAtomInfo::getAtom(const size_t i)
{
  return *myAtoms[i];
}

const common::Atom & BuildAtomInfo::getAtom(const size_t i) const
{
  return *myAtoms[i];
}

common::AtomGroup * BuildAtomInfo::getGroup()
{
  return myGroup.get();
}

const common::AtomGroup * BuildAtomInfo::getGroup() const
{
  return myGroup.get();
}

void BuildAtomInfo::setFixed(const bool fixed)
{
  myFixed = fixed;
}

bool BuildAtomInfo::isFixed() const
{
  return myFixed;
}

int BuildAtomInfo::getMultiplicity() const
{
  return myMultiplicity;
}

void BuildAtomInfo::setMultiplicity(const int multiplicity)
{
  myMultiplicity = multiplicity;
}

const BuildAtomInfo::OpMask & BuildAtomInfo::getOpMask() const
{
  return myOpMask;
}

void BuildAtomInfo::setOperatorsMask(const OpMask & opMask)
{
  myOpMask = opMask;
}

BuildAtomInfo::BuildAtomInfo(common::Atom & atom):
myFixed(false),
myMultiplicity(1)
{
  myAtoms.push_back(&atom);
}

void BuildAtomInfo::addAtom(common::Atom & atom)
{
  myAtoms.push_back(&atom);
}

void BuildAtomInfo::removeAtom(common::Atom & atom)
{
  const AtomsList::iterator it = ::std::find(myAtoms.begin(), myAtoms.end(), &atom);
  SSLIB_ASSERT(it != myAtoms.end());
  myAtoms.erase(it);
}

}
}
