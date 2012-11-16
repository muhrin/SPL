/*
 * StructureDescriptionMap.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "build_cell/StructureDescriptionMap.h"

#include <boost/foreach.hpp>

#include "build_cell/AtomGroupDescription.h"
#include "build_cell/StructureDescription.h"
#include "common/Atom.h"

namespace sstbx
{
namespace build_cell
{

StructureDescriptionMap::StructureDescriptionMap(const StructureDescription & description, common::Structure & structure):
myStructureDescription(description),
myStructure(structure)
{}

const StructureDescription & StructureDescriptionMap::getStructureDescription() const
{
  return myStructureDescription;
}

common::Structure & StructureDescriptionMap::getStructure()
{
  return myStructure;
}

const common::Structure & StructureDescriptionMap::getStructure() const
{
  return myStructure;
}

void StructureDescriptionMap::insert(
  const AtomGroupDescription * const  atomGroupDescription,
  const AtomsDescription * const      atomDescription,
  ::sstbx::common::Atom * const       atom)
{
  // Insert the entry into both our maps
  myAtomsMap.insert(AtomsMap::value_type(atomDescription, atom));
  myGroupAtomsMap.insert(GroupAtomsMap::value_type(atomGroupDescription, atom));
}

StructureDescriptionMap::AtomsRange
StructureDescriptionMap::getAtoms(const AtomsDescription * atomDescription)
{
  return AtomsRange(myAtomsMap.equal_range(atomDescription));
}

StructureDescriptionMap::AtomsRangeConst
StructureDescriptionMap::getAtoms(const AtomsDescription * atomDescription) const
{
  return AtomsRangeConst(myAtomsMap.equal_range(atomDescription));
}

StructureDescriptionMap::GroupAtomsRange
StructureDescriptionMap::getAtoms(const AtomGroupDescription * atomGroupDescription)
{
  return GroupAtomsRange(myGroupAtomsMap.equal_range(atomGroupDescription));
}

StructureDescriptionMap::GroupAtomsRangeConst
StructureDescriptionMap::getAtoms(const AtomGroupDescription * atomGroupDescription) const
{
  return GroupAtomsRangeConst(myGroupAtomsMap.equal_range(atomGroupDescription));
}

size_t StructureDescriptionMap::getDescriptionsAndAtoms(
  const AtomGroupDescription * groupDescription,
  DescriptionsAndAtoms &       descriptionsAndAtoms,
  const unsigned int           maxDepth)
{
  size_t numFound = 0;

  common::Atom * atom;
  BOOST_FOREACH(const AtomsDescription & atomDescription, groupDescription->getChildAtoms())
  {
    const AtomsRange range = getAtoms(&atomDescription);

    BOOST_FOREACH(atom, range)
    {
      descriptionsAndAtoms.push_back(DescriptionAndAtom(&atomDescription, atom));
      ++numFound;
    }
  }

  if(maxDepth > 0)
  {
    BOOST_FOREACH(const AtomGroupDescription & childGroup, groupDescription->getChildGroups())
    {
      numFound += getDescriptionsAndAtoms(&childGroup, descriptionsAndAtoms, maxDepth - 1);
    }
  }
  return numFound;
}

size_t StructureDescriptionMap::getDescriptionsAndAtoms(
  const AtomGroupDescription * groupDescription,
  ConstDescriptionsAndAtoms &  descriptionsAndAtoms,
  const unsigned int           maxDepth) const
{
  size_t numFound = 0;

  common::Atom * atom;
  BOOST_FOREACH(const AtomsDescription & atomDescription, groupDescription->getChildAtoms())
  {
    const AtomsRangeConst range = getAtoms(&atomDescription);

    BOOST_FOREACH(atom, range)
    {
      descriptionsAndAtoms.push_back(DescriptionAndAtom(&atomDescription, atom));
      ++numFound;
    }
  }

  if(maxDepth > 0)
  {
    BOOST_FOREACH(const AtomGroupDescription & childGroup, groupDescription->getChildGroups())
    {
      numFound += getDescriptionsAndAtoms(&childGroup, descriptionsAndAtoms, maxDepth - 1);
    }
  }
  return numFound;
}


size_t StructureDescriptionMap::getDescriptionsAndAtomsBelow(
  const AtomGroupDescription * groupDescription,
  DescriptionsAndAtoms &       descriptionsAndAtoms,
  const unsigned int           maxDepth)
{
  size_t numFound = 0;
  BOOST_FOREACH(const AtomGroupDescription & childGroup, groupDescription->getChildGroups())
  {
    numFound += getDescriptionsAndAtoms(&childGroup, descriptionsAndAtoms, maxDepth);
  }
  return numFound;
}

size_t StructureDescriptionMap::getDescriptionsAndAtomsBelow(
  const AtomGroupDescription * groupDescription,
  ConstDescriptionsAndAtoms &       descriptionsAndAtoms,
  const unsigned int           maxDepth) const
{
  size_t numFound = 0;
  BOOST_FOREACH(const AtomGroupDescription & childGroup, groupDescription->getChildGroups())
  {
    numFound += getDescriptionsAndAtoms(&childGroup, descriptionsAndAtoms, maxDepth);
  }
  return numFound;
}

size_t StructureDescriptionMap::getAtomsBelow(
  const AtomGroupDescription * groupDescription,
  ::std::vector<const common::Atom *> & atoms) const
{
  size_t numFound = 0;

  const common::Atom * atom;
  BOOST_FOREACH(const AtomGroupDescription & childGroup, groupDescription->getChildGroups())
  {
    const GroupAtomsRangeConst range = getAtoms(&childGroup);

    BOOST_FOREACH(atom, range)
    {
      atoms.push_back(atom);
      ++numFound;
    }
  }

  return numFound;
}

StructureDescriptionMap::AtomsMapIterator StructureDescriptionMap::beginAtoms()
{
  return myAtomsMap.begin();
}

StructureDescriptionMap::AtomsMapIterator StructureDescriptionMap::endAtoms()
{
  return myAtomsMap.end();
}

StructureDescriptionMap::GroupAtomsMapIterator StructureDescriptionMap::beginGroups()
{
  return myGroupAtomsMap.begin();
}

StructureDescriptionMap::GroupAtomsMapIterator StructureDescriptionMap::endGroups()
{
  return myGroupAtomsMap.end();
}

size_t StructureDescriptionMap::getNumAtoms(const AtomsRange & range) const
{
  return ::boost::distance(range);
}

size_t StructureDescriptionMap::getNumAtoms(const AtomsRangeConst & range) const
{
  return ::boost::distance(range);
}

size_t StructureDescriptionMap::getNumAtoms(const GroupAtomsRange & range) const
{
  return ::boost::distance(range);
}

size_t StructureDescriptionMap::getNumAtoms(const GroupAtomsRangeConst & range) const
{
  return ::boost::distance(range);
}

size_t StructureDescriptionMap::getNumAtoms(
  const AtomGroupDescription & groupDescription,
  const unsigned int           maxDepth
) const
{
  size_t numAtoms = myGroupAtomsMap.count(&groupDescription);
  if(maxDepth > 0)
  {
    BOOST_FOREACH(const AtomGroupDescription & group, groupDescription.getChildGroups())
    {
      numAtoms += getNumAtoms(group, maxDepth - 1);
    }
  }
  return numAtoms;
}

size_t StructureDescriptionMap::getAtomPositions(
  const AtomGroupDescription & groupDescription,
  ::arma::mat &                posMatrix,
  const unsigned int           maxDepth,
  const size_t                 startCol) const
{
  const size_t numAtoms = getNumAtoms(groupDescription, maxDepth);

  if(posMatrix.n_rows != 3 || posMatrix.n_cols + startCol < numAtoms)
  {
    posMatrix.set_size(3, numAtoms + startCol);
  }

  // First get the atoms of this group
  size_t col = startCol;
  const GroupAtomsRangeConst range = getAtoms(&groupDescription);
  const common::Atom * atom;
  BOOST_FOREACH(atom, range)
  {
    posMatrix.col(col) = atom->getPosition();
    ++col;
  }

  if(maxDepth > 0)
  {
    BOOST_FOREACH(const AtomGroupDescription & childGroup, groupDescription.getChildGroups())
    {
      col += getAtomPositions(childGroup, posMatrix, maxDepth - 1, col);
    }
  }

  return numAtoms;
}

}
}
