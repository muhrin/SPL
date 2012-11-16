/*
 * StructureDescriptionMap.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_DESCRIPTION_MAP_H
#define STRUCTURE_DESCRIPTION_MAP_H

// INCLUDES ///////////////////
#include <functional>
#include <map>
#include <utility>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <armadillo>

#include "build_cell/AtomGroupDescription.h"
#include "common/Types.h"

// FORWARD DECLARES ///////////
namespace sstbx {
namespace build_cell {
class AtomsDescription;
class StructureDescription;
}

namespace common {
class Atom;
class Structure;
}
}

namespace sstbx {
namespace build_cell {

template <typename T1, typename T2>
class TakeSecond : public ::std::unary_function< const ::std::pair<T1, T2> &, T2>
{
public:
  T2 operator() (const ::std::pair<T1, T2> & pair) const
  {
    return pair.second;
  }
};

class StructureDescriptionMap
{
private:

  typedef ::std::multimap<const AtomsDescription *, ::sstbx::common::Atom *>          AtomsMap;
  typedef TakeSecond<const AtomsDescription *, ::sstbx::common::Atom *>               AtomsMapTransformer;

  typedef ::std::multimap<const AtomGroupDescription *, ::sstbx::common::Atom *>      GroupAtomsMap;
  typedef TakeSecond<const AtomGroupDescription *, ::sstbx::common::Atom *>           GroupAtomsTransformer;

public:


  // ATOM ITERATORS AND RANGES ///////////////////////
  typedef AtomsMap::iterator                                                       AtomsMapIterator;
  typedef AtomsMap::const_iterator                                                 AtomsMapConstIterator;
  typedef ::boost::transform_iterator<AtomsMapTransformer, AtomsMapIterator>       AtomsIterator;
  typedef ::boost::transform_iterator<AtomsMapTransformer, AtomsMapConstIterator>  AtomsConstIterator;
  typedef ::boost::iterator_range<AtomsIterator>                                   AtomsRange;
  typedef ::boost::iterator_range<AtomsConstIterator>                              AtomsRangeConst;

  // GROUP ATOM ITERATORS AND RANGES ////////////////////
  typedef GroupAtomsMap::iterator                                                         GroupAtomsMapIterator;
  typedef GroupAtomsMap::const_iterator                                                   GroupAtomsMapConstIterator;
  typedef ::boost::transform_iterator<GroupAtomsTransformer, GroupAtomsMapIterator>       GroupAtomsIterator;
  typedef ::boost::transform_iterator<GroupAtomsTransformer, GroupAtomsMapConstIterator>  GroupAtomsConstIterator;
  typedef ::boost::iterator_range<GroupAtomsIterator>                                     GroupAtomsRange;
  typedef ::boost::iterator_range<GroupAtomsConstIterator>                                GroupAtomsRangeConst;

  // ATOMS AND DESCRIPTIONS PAIR /////////////////////////
  typedef ::std::pair<const AtomsDescription *, common::Atom *>                   DescriptionAndAtom;
  typedef ::std::vector<DescriptionAndAtom>                                       DescriptionsAndAtoms;
  typedef ::std::pair<const AtomsDescription *, const common::Atom *>             ConstDescriptionAndAtom;
  typedef ::std::vector<ConstDescriptionAndAtom>                                  ConstDescriptionsAndAtoms;


  StructureDescriptionMap(const StructureDescription & description, common::Structure & structure);

  const StructureDescription & getStructureDescription() const;

  common::Structure & getStructure();
  const common::Structure & getStructure() const;

  void insert(
    const AtomGroupDescription * const  atomGroupDescription,
    const AtomsDescription * const      atomDescription,
    ::sstbx::common::Atom * const       atom);

  // Access atoms //////////////////////////////////////////
  AtomsRange getAtoms(const AtomsDescription * atomDescription);
  AtomsRangeConst getAtoms(const AtomsDescription * atomDescription) const;
  GroupAtomsRange getAtoms(const AtomGroupDescription * groupDescription);
  GroupAtomsRangeConst getAtoms(const AtomGroupDescription * groupDescription) const;



  /**
  /* Get all the atom descriptions along with the corresponding atoms
  /* within this group and all child groups up to the maximum depth.
  /**/
  size_t getDescriptionsAndAtoms(
    const AtomGroupDescription * groupDescription,
    DescriptionsAndAtoms &       descriptionsAndAtoms,
    const unsigned int           maxDepth = AtomGroupDescription::DEPTH_LOCAL_GROUP_ONLY);

  size_t getDescriptionsAndAtoms(
    const AtomGroupDescription * groupDescription,
    ConstDescriptionsAndAtoms &  descriptionsAndAtoms,
    const unsigned int           maxDepth = AtomGroupDescription::DEPTH_LOCAL_GROUP_ONLY) const;

  /**
  /* Get all the atom descriptions along with the corresponding atoms
  /* within all the groups below this one up to the maximum depth, counting
  /* from the groups below this one.
  /**/
  size_t getDescriptionsAndAtomsBelow(
    const AtomGroupDescription * groupDescription,
    DescriptionsAndAtoms &       descriptionsAndAtoms,
    const unsigned int           maxDepth = AtomGroupDescription::DEPTH_LOCAL_GROUP_ONLY);

  size_t getDescriptionsAndAtomsBelow(
    const AtomGroupDescription * groupDescription,
    ConstDescriptionsAndAtoms &  descriptionsAndAtoms,
    const unsigned int           maxDepth = AtomGroupDescription::DEPTH_LOCAL_GROUP_ONLY) const;

  size_t getAtomsBelow(const AtomGroupDescription * groupDescription, ::std::vector<const common::Atom *> & atoms) const;

  // Iterate atoms or groups ///////////////////////
  AtomsMapIterator beginAtoms();
  AtomsMapIterator endAtoms();

  GroupAtomsMapIterator beginGroups();
  GroupAtomsMapIterator endGroups();

  // Find out numbers of atoms ///////////////////////
  size_t getNumAtoms(const AtomsRange & range) const;
  size_t getNumAtoms(const AtomsRangeConst & range) const;
  size_t getNumAtoms(const GroupAtomsRange & range) const;
  size_t getNumAtoms(const GroupAtomsRangeConst & range) const;

  size_t getNumAtoms(
    const AtomGroupDescription & groupDescription,
    const unsigned int           maxDepth = AtomGroupDescription::DEPTH_LOCAL_GROUP_ONLY) const;

  // Get atom positions ///////////////////////////////
  size_t getAtomPositions(
    const AtomGroupDescription & groupDescription,
    ::arma::mat &                posMatrix,
    const unsigned int           maxDepth = AtomGroupDescription::DEPTH_LOCAL_GROUP_ONLY,
    const size_t                 startCol = 0) const;

private:

  const StructureDescription &  myStructureDescription;
  common::Structure &           myStructure;
  AtomsMap                      myAtomsMap;
  GroupAtomsMap                 myGroupAtomsMap;

};

}
}

#endif /* STRUCTURE_DESCRIPTION_MAP_H */
