/*
 * PermutationRange.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef PERMUTATION_RANGE_H
#define PERMUTATION_RANGE_H

// INCLUDES /////////////////////////////////////////////
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_array.hpp>
#include <boost/range/iterator_range.hpp>

#include "spl/SSLib.h"
#include "spl/utility/IndexRemappingView.h"

#define SSLIB_PERMUTATION_RANGE_DEBUG (0 & SSLIB_DEBUG)

#if SSLIB_PERMUTATION_RANGE_DEBUG
#  include <iostream>
#endif

namespace spl {
namespace utility {

// FORWARD DECLARATIONS ////////////////////////////////////
template <typename Object, typename Element>
class PermutationRange;

namespace utility_detail {


template <typename Object, typename Element>
class PermutationIterator : public ::boost::iterator_facade<
  PermutationIterator<Object, Element>,
  utility::IndexRemappingViewShared<Object, Element>,
  ::boost::bidirectional_traversal_tag,
  utility::IndexRemappingViewShared<Object, Element>
>
{
  typedef ::boost::iterator_facade<
    PermutationIterator<Object, Element>,
    utility::IndexRemappingViewShared<Object, Element>,
    ::boost::bidirectional_traversal_tag,
    utility::IndexRemappingViewShared<Object, Element>
  > base_t;

public:

  typedef typename base_t::value_type value_type;
  typedef typename base_t::difference_type difference_type;
  typedef typename base_t::reference reference;

  static PermutationIterator<Object, Element> makeBegin(Object & object, const size_t numElements);
  static PermutationIterator<Object, Element> makeEnd(Object & object, const size_t numElements);

  PermutationIterator(const PermutationIterator<Object, Element> & toCopy):
    myNumElements(toCopy.myNumElements),
    myIndexMapping(toCopy.myIndexMapping),
    myIndexDirections(toCopy.myIndexDirections),
    myRemappingView(toCopy.myRemappingViewPtr->getObject(), myIndexMapping),
    myRemappingViewPtr(&myRemappingView)
  {   
  }

private:

  PermutationIterator(Object & object, const size_t numElements);

  void initStart();
  void initEnd();

  utility::IndexRemappingViewShared<Object, Element> dereference() const
  {
    return myRemappingView;
  }

  // From iterator_facade ///////////////

  void increment()
  {
    ptrdiff_t largest = ::std::numeric_limits<ptrdiff_t>::min();
    int largestIdx = -1;
    for(size_t i = 0; i < myNumElements; ++i)
    {
      if(myIndexDirections[i] != 0 &&
        myIndexMapping[i] > largest)
      {
        largest = myIndexMapping[i];
        largestIdx = i;
      }
    }

    if(largestIdx == -1)
    {
      // We've reached the end of the permutation
      initEnd();
    }
    else
    {
      const ptrdiff_t direction = myIndexDirections[largestIdx];
      const size_t newIdx = largestIdx + direction;

      if(newIdx == 0 ||
        newIdx == (myNumElements - 1) ||
        // CAREFUL: This has to be the last statement in the OR sequence to avoid
        // accessing invalid array index
        myIndexMapping[newIdx + direction] > largest)
      {
        myIndexDirections[largestIdx] = 0;
      }

      ptrdiff_t tmpSwap;
      // Swap the indices
      tmpSwap = myIndexMapping[newIdx];
      myIndexMapping[newIdx] = largest;
      myIndexMapping[largestIdx] = tmpSwap;

      // Swap the directions
      tmpSwap = myIndexDirections[newIdx];
      myIndexDirections[newIdx] = myIndexDirections[largestIdx];
      myIndexDirections[largestIdx] = tmpSwap;  

      postStep(largestIdx, largest);
    }

#if SSLIB_PERMUTATION_RANGE_DEBUG
    for(size_t i = 0; i < myNumElements; ++i)
    {
      if(myIndexDirections[i] == 1)
        ::std::cout << " +";
      else if(myIndexDirections[i] == -1)
        ::std::cout << " -";
      else
        ::std::cout << "  ";

      ::std::cout << myIndexMapping[i];
    }
    ::std::cout << ::std::endl;
#endif
  }

  void decrement()
  {
    // TODO
  }

  void advance(difference_type & offset)
  {
    // TODO
  }

  bool equal(const PermutationIterator<Object, Element> & other) const
  {
    if(myNumElements != other.myNumElements)
      return false;

    for(size_t i = 0; i < myNumElements; ++i)
    {
      if(myIndexMapping[i] != other.myIndexMapping[i])
        return false;
    }

    return true;
  }

  // End from iterator_facade ////////////////

  void inline postStep(const size_t elementIndex, const ptrdiff_t elementValue)
  {
    size_t i;
    for(i = 0; i < elementIndex; ++i)
    {
      if(myIndexMapping[i] > elementValue)
        myIndexDirections[i] = +1;
    }
    for(i = elementIndex + 1; i < myNumElements; ++i)
    {
      if(myIndexMapping[i] > elementValue)
        myIndexDirections[i] = -1;
    }
  }

  const size_t                        myNumElements;
  ::std::vector<ptrdiff_t>            myIndexMapping;
  ::std::vector<short>                myIndexDirections;
  IndexRemappingViewShared<Object, Element> myRemappingView;
  IndexRemappingViewShared<Object, Element> * myRemappingViewPtr;

  friend class ::boost::iterator_core_access;
};

template <typename Object, typename Element>
PermutationIterator<Object, Element>::PermutationIterator(Object & object, const size_t numElements):
myNumElements(numElements),
myIndexMapping(numElements),
myIndexDirections(numElements),
myRemappingView(object, myIndexMapping),
myRemappingViewPtr(&myRemappingView)
{
}

template <typename Object, typename Element>
void PermutationIterator<Object, Element>::initStart()
{
  // Initially start at the configuration iter[i] = i
  for(size_t i = 0; i < myNumElements; ++i)
  {
    myIndexMapping[i] = i;
  }

  // The first value has zero direction
  myIndexDirections[0] = 0;
  // All the rest have a negative direction
  ::std::fill(myIndexDirections.begin() + 1, myIndexDirections.end(), -1);
}

template <typename Object, typename Element>
void PermutationIterator<Object, Element>::initEnd()
{
  // Fill the index mapping array with the end value i.e. the size
  ::std::fill(myIndexMapping.begin(), myIndexMapping.end(), myNumElements);
  // Direction is now zero for all elements
  ::std::fill(myIndexDirections.begin(), myIndexDirections.end(), 0);
}

template <typename Object, typename Element>
PermutationIterator<Object, Element> PermutationIterator<Object, Element>::makeBegin(Object & object, const size_t numElements)
{
  PermutationIterator<Object, Element> perm(object, numElements);
  perm.initStart();
  return perm;
}

template <typename Object, typename Element>
PermutationIterator<Object, Element> PermutationIterator<Object, Element>::makeEnd(Object & object, const size_t numElements)
{
  PermutationIterator<Object, Element> perm(object, numElements);
  perm.initEnd();
  return perm;
}


} // namespace utility_detail


template <typename Object, typename Element>
class PermutationRange :
  public ::boost::iterator_range<utility_detail::PermutationIterator<Object, Element> >
{
public:

  typedef utility_detail::PermutationIterator<Object, Element> iterator_t;
  typedef ::boost::iterator_range<iterator_t> base_t;

  PermutationRange(Object & object, const size_t numElements):
    base_t(
      iterator_t::makeBegin(object, numElements),
      iterator_t::makeEnd(object, numElements)
    )
  {}

};

template <typename T>
class PermutationRange< ::std::vector<T>, T> :
  public ::boost::iterator_range<utility_detail::PermutationIterator< ::std::vector<T>, T> >
{
public:

  typedef utility_detail::PermutationIterator< ::std::vector<T>, T> iterator_t;
  typedef ::boost::iterator_range<iterator_t> base_t;

  PermutationRange(::std::vector<T> & vector):
    base_t(
      iterator_t::makeBegin(vector, vector.size()),
      iterator_t::makeEnd(vector, vector.size())
    )
  {}
};


// Specialisation for pointer type
template <typename T>
class PermutationRange<T *, T> :
  public ::boost::iterator_range<utility_detail::PermutationIterator<T *, T> >
{
public:

  typedef utility_detail::PermutationIterator<T *, T> iterator_t;
  typedef ::boost::iterator_range<iterator_t> base_t;

  PermutationRange(T * ptr, const size_t numElements);

};

template <typename T>
PermutationRange<T *, T>::PermutationRange(T * ptr, const size_t numElements):
PermutationRange<T *, T>::base_t(iterator_t::makeBegin(ptr, numElements),
                                   iterator_t::makeEnd(ptr, numElements))
{}

} // namespace utility
} // namespace spl


#endif /* PERMUTATION_RANGE_H */
