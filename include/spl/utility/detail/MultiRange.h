/*
 * MultiRange.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MULTI_RANGE_DETAIL_H
#define MULTI_RANGE_DETAIL_H

// INCLUDES /////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {
//
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//MultiRangeAccess<Value, Traversal, Reference, Difference>::MultiRangeAccess(
//  MultiRangeIteratorType & iterator):
//myIterator(iterator)
//{}
//
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//Reference
//MultiRangeAccess<Value, Traversal, Reference, Difference>::operator[](const size_t idx)
//{
//  return *myIterator.getIterator(idx);
//}
//
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//MultiRangeIterator<Value, Traversal, Reference, Difference>::MultiRangeIterator(
//  Iterators & iterators,
//  const size_t currentIterating):
//myIterators(iterators)
//{
//}
//
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//typename MultiRangeIterator<Value, Traversal, Reference, Difference>::AnyIterator &
//MultiRangeIterator<Value, Traversal, Reference, Difference>::getIterator(const size_t idx)
//{
//  return myIterators[idx];
//}

//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//const typename MultiRangeIterator<Value, Traversal, Reference, Difference>::AnyIterator &
//MultiRangeIterator<Value, Traversal, Reference, Difference>::getIterator(const size_t idx) const
//{
//  return myIterators[idx];
//}

template <
  class Value,
  class Traversal,
  class Reference,
  class Difference
>
template <class Range>
void MultiRange<Value, Traversal, Reference, Difference>::pushBack(const Range & range)
{
  myRanges.push_back(AnyRange(range));
}

} // namespace utility
} // namespace spl


#endif /* MULTI_RANGE_DETAIL_H */
