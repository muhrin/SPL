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

#include "common/Structure.h"
#include "utility/IBufferedComparator.h"
#include "utility/TransformFunctions.h"


// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace utility {
class IStructureComparator;
}
}


namespace sstbx {
namespace utility {

namespace detail {

template <typename Key>
class UniqueStructureSetBase
{
protected:
  typedef IBufferedComparator::ComparisonDataHandle ComparisonDataHandle;
  typedef ::std::map<Key, ComparisonDataHandle> StructureMap;

public:

  typedef utility::TakeFirst<Key, ComparisonDataHandle> TakeFirst;
  typedef utility::TakeFirst<const Key, ComparisonDataHandle> TakeFirstConst;

  typedef Key value_type;
  // Iterators ///
  typedef boost::transform_iterator<TakeFirst, typename StructureMap::iterator> iterator;
  typedef boost::transform_iterator<TakeFirstConst, typename StructureMap::const_iterator> const_iterator;
  typedef boost::transform_iterator<TakeFirst, typename StructureMap::reverse_iterator> reverse_iterator;
  typedef boost::transform_iterator<TakeFirstConst, typename StructureMap::const_reverse_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> insert_return_type;

  typedef typename StructureMap::size_type size_type;

  typedef std::pair<Key, bool> ReturnPair;

  UniqueStructureSetBase(const IStructureComparator & comparator);

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

  UniqueStructureSet(const IStructureComparator & comparator);

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

  UniqueStructureSet(const IStructureComparator & comparator);

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

#include "utility/detail/UniqueStructureSet.h"

#endif /* UNIQUE_STRUCTURE_SET_H */
