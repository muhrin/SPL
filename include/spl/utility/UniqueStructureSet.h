/*
 * UniqueStructureSet.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef UNIQUE_STRUCTURE_SET_H
#define UNIQUE_STRUCTURE_SET_H

// INCLUDES /////////////////////////////////////////////
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "spl/common/Structure.h"
#include "spl/utility/IBufferedComparator.h"
#include "spl/utility/TransformFunctions.h"
#include "spl/utility/UtilityFwd.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {
namespace detail {

template <typename Key>
class UniqueStructureSetBase
{
protected:
  typedef IBufferedComparator::ComparisonDataHandle ComparisonDataHandle;
  typedef ::std::map<Key, ComparisonDataHandle> StructureMap;

public:
  typedef utility::TakeFirst<typename StructureMap::value_type> TakeFirst;
  typedef utility::TakeFirst<const typename StructureMap::value_type> TakeFirstConst;

  typedef Key value_type;
  // Iterators ///
  typedef boost::transform_iterator<TakeFirst, typename StructureMap::iterator> iterator;
  typedef boost::transform_iterator<TakeFirstConst, typename StructureMap::const_iterator> const_iterator;
  typedef boost::transform_iterator<TakeFirst, typename StructureMap::reverse_iterator> reverse_iterator;
  typedef boost::transform_iterator<TakeFirstConst, typename StructureMap::const_reverse_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> insert_return_type;

  typedef typename StructureMap::size_type size_type;

  typedef std::pair<Key, bool> ReturnPair;

  explicit UniqueStructureSetBase(IStructureComparatorPtr comparator);
  explicit UniqueStructureSetBase(const IStructureComparator & comparator);

  // Iterators /////////////////////////
  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;

  reverse_iterator rend();
  const_reverse_iterator rend() const;

  // Capacity ////////////////////////////
  bool empty() const;
  size_type size() const;
  size_type max_size() const;

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last);
  void erase(iterator position);
  
	void clear();

  // Operations ////////////////////////////
  iterator find(const Key & key);
  const_iterator find(const Key key) const;

protected:
  typedef ::boost::shared_ptr<IBufferedComparator> Comparator;
  typedef std::pair<typename StructureMap::iterator, bool> MapInsertReturn;

  MapInsertReturn insertStructure(const Key & key, common::Structure & correspondingStructure);

private:

  // WARNING: Order is important.  When using initialiser list the order of declaration
  // will be used (not the order of the list itself).  Therefore the owned comparator must
  // come first, then the comparator.
  IStructureComparatorPtr myOwnedComparator;
  const Comparator myComparator;
  StructureMap myStructures;
};


} // namespace detail

template <typename Key = common::Structure *>
class UniqueStructureSet : public detail::UniqueStructureSetBase<Key>
{
private:
  typedef detail::UniqueStructureSetBase<Key> Base;
  typedef typename Base::StructureMap StructureMap;

public:

  // Iterators ///
  typedef typename Base::iterator iterator;
  typedef typename Base::const_iterator const_iterator;
  typedef typename Base::reverse_iterator reverse_iterator;
  typedef typename Base::const_reverse_iterator const_reverse_iterator;
  typedef typename Base::insert_return_type insert_return_type;

  typedef typename StructureMap::size_type size_type;

  explicit UniqueStructureSet(IStructureComparatorPtr comparator);
  explicit UniqueStructureSet(const IStructureComparator & comparator);

  // Modifiers ///////////////////////////
  insert_return_type insert(const Key & key, common::Structure & correspondingStructure);
private:
  typedef typename Base::MapInsertReturn MapInsertReturn;
};

template <>
class UniqueStructureSet<common::Structure *> : public detail::UniqueStructureSetBase<common::Structure *>
{
private:
  typedef common::Structure * Key;
  typedef detail::UniqueStructureSetBase<Key> Base;
  typedef Base::StructureMap StructureMap;
public:


  typedef Key value_type;
  // Iterators ///
  typedef Base::iterator iterator;
  typedef Base::const_iterator const_iterator;
  typedef Base::reverse_iterator reverse_iterator;
  typedef Base::const_reverse_iterator const_reverse_iterator;
  typedef Base::insert_return_type insert_return_type;

  typedef StructureMap::size_type size_type;
  typedef std::pair<Key, bool> ReturnPair;

  explicit UniqueStructureSet(IStructureComparatorPtr comparator);
  explicit UniqueStructureSet(const IStructureComparator & comparator);

  // Modifiers ///////////////////////////
  insert_return_type insert(common::Structure * structure);
private:
  typedef Base::MapInsertReturn MapInsertReturn;
};

//template <class InputIterator>
//void UniqueStructureSet::insert(const InputIterator first, const InputIterator last)
//{
//  // Insert any unique structures in the range
//  for(InputIterator it = first; it != last; ++it)
//    insertStructure(*it);
//}

}
}

#include "spl/utility/detail/UniqueStructureSet.h"

#endif /* UNIQUE_STRUCTURE_SET_H */
