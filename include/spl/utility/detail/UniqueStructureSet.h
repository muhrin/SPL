/*
 * UniqueStructureSetBase.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/utility/IStructureComparator.h"

namespace spl {
namespace utility {
namespace detail {

template <typename Key>
UniqueStructureSetBase<Key>::UniqueStructureSetBase(IStructureComparatorPtr comparator):
myOwnedComparator(comparator),
myComparator(myOwnedComparator->generateBuffered())
{}

template <typename Key>
UniqueStructureSetBase<Key>::UniqueStructureSetBase(const IStructureComparator & comparator):
myComparator(comparator.generateBuffered())
{}

template <typename Key>
typename UniqueStructureSetBase<Key>::iterator
UniqueStructureSetBase<Key>::begin()
{
  return iterator(myStructures.begin(), TakeFirst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::const_iterator
UniqueStructureSetBase<Key>::begin() const
{
  return const_iterator(myStructures.begin(), TakeFirstConst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::iterator
UniqueStructureSetBase<Key>::end()
{
  return iterator(myStructures.end(), TakeFirst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::const_iterator
UniqueStructureSetBase<Key>::end() const
{
  return const_iterator(myStructures.end(), TakeFirstConst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::reverse_iterator
UniqueStructureSetBase<Key>::rbegin()
{
  return reverse_iterator(myStructures.rbegin(), TakeFirst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::const_reverse_iterator
UniqueStructureSetBase<Key>::rbegin() const
{
  return const_reverse_iterator(myStructures.rbegin(), TakeFirstConst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::reverse_iterator
UniqueStructureSetBase<Key>::rend()
{
  return reverse_iterator(myStructures.rend(), TakeFirst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::const_reverse_iterator
UniqueStructureSetBase<Key>::rend() const
{
  return const_reverse_iterator(myStructures.rend(), TakeFirstConst());
}

template <typename Key>
bool UniqueStructureSetBase<Key>::empty() const
{
  return myStructures.empty();
}

template <typename Key>
typename UniqueStructureSetBase<Key>::size_type
UniqueStructureSetBase<Key>::size() const
{
  return myStructures.size();
}

template <typename Key>
typename UniqueStructureSetBase<Key>::size_type
UniqueStructureSetBase<Key>::max_size() const
{
  return myStructures.max_size();
}

template <typename Key>
void UniqueStructureSetBase<Key>::erase(
  typename UniqueStructureSetBase<Key>::iterator position)
{
  myStructures.erase(position.base());
}

template <typename Key>
void UniqueStructureSetBase<Key>::clear()
{
	myStructures.clear();
}

template <typename Key>
typename UniqueStructureSetBase<Key>::iterator
UniqueStructureSetBase<Key>::find(const Key & key)
{
  return iterator(myStructures.find(key), TakeFirst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::const_iterator
UniqueStructureSetBase<Key>::find(const Key key) const
{
  return const_iterator(myStructures.find(key), TakeFirstConst());
}

template <typename Key>
typename UniqueStructureSetBase<Key>::MapInsertReturn
UniqueStructureSetBase<Key>::insertStructure(const Key & key, common::Structure & correspondingStructure)
{
  MapInsertReturn returnPair;
  // .second is used to indiate that a new structure was inserted
  returnPair.second = true;

  ComparisonDataHandle handle(myComparator->generateComparisonData(correspondingStructure));

  // Check if we have a structure like this already
  for(typename StructureMap::iterator it = myStructures.begin(), end = myStructures.end();
    it != end; ++it)
  {
    if(myComparator->areSimilar(handle, it->second))
    {
      returnPair.first = it;
      returnPair.second = false;
      break;
    }
  }

  // If it is not like any of those we have already then insert it
  if(returnPair.second)
  {
    returnPair = myStructures.insert(typename StructureMap::value_type(key, handle));
  }
  else
  {
    // Tell the comparator the remove the comparison data for this structure as we
    // are not storing it.
    handle.release();
  }

  return returnPair;
}

} // namespace detail

template <typename Key>
UniqueStructureSet<Key>::UniqueStructureSet(IStructureComparatorPtr comparator):
detail::UniqueStructureSetBase<Key>(comparator)
{}

template <typename Key>
UniqueStructureSet<Key>::UniqueStructureSet(const IStructureComparator & comparator):
detail::UniqueStructureSetBase<Key>(comparator)
{}

template <typename Key>
typename UniqueStructureSet<Key>::insert_return_type
UniqueStructureSet<Key>::insert(const Key & key, common::Structure & correspondingStructure)
{
  MapInsertReturn pair = this->insertStructure(key, correspondingStructure);
  return insert_return_type(iterator(pair.first), pair.second);
}


}
}
