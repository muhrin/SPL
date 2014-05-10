/*
 * MultiIdxRange.h
 *
 * Range in multidimensional index space.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MULTI_IDX_RANGE_H
#define MULTI_IDX_RANGE_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

#include "spl/SSLibAssert.h"
#include "spl/utility/MultiIdx.h"

#ifdef _MSC_VER
// Disable warning about passing this pointer to this in initialiser list
#  pragma warning( push )
#  pragma warning( disable : 4355 )
#endif

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template< typename Integer>
  class MultiIdxRange;

namespace utility_detail {

template< typename Integer>
  class ConstMultiIdxIterator : public ::boost::iterator_facade<
      ConstMultiIdxIterator< Integer>, const MultiIdx< Integer>,
      ::boost::random_access_traversal_tag, const MultiIdx< Integer> &,
      MultiIdx< Integer> >
  {
    typedef ::boost::iterator_facade< ConstMultiIdxIterator< Integer>,
        const MultiIdx< Integer>, ::boost::random_access_traversal_tag,
        const MultiIdx< Integer> &, MultiIdx< Integer> > base_t;

  public:
    typedef typename base_t::value_type value_type;
    typedef typename base_t::difference_type difference_type;
    typedef typename base_t::reference reference;

    ConstMultiIdxIterator(value_type x, const MultiIdxRange< Integer> & range) :
        myValue(x), myRange(range)
    {
    }

  private:

    void
    increment()
    {
      bool increment = true;
      for(size_t i = 0; increment && i < myRange.dims(); ++i)
      {
        // Increment the position vector and check it does not exceed
        // the max number for that index position
        if(++myValue[i] >= myRange.myEnd[i])
          myValue[i] = myRange.myBegin[i];
        else
          // Successfully moved one position on in index space
          increment = false;
      }

      // Do we still need to increment even though we have covered all digits?
      if(increment)
        // Then we have reached the end
        myValue = myRange.myEnd;
    }

    void
    decrement()
    {
      // TODO: Work on this more.  Need to deal with situation where it == end
      bool decrement = true;
      for(size_t i = 0; decrement && i > myRange.dims(); ++i)
      {
        // Decrement the position vector and check it does not go below
        // the minimum number for that index position
        if(--myValue[i] < myRange.myBegin[i])
          myValue[i] = myRange.myEnd[i] - 1; // have to do -1 as the range is closed
        else
          // Successfully moved one position on in index space
          decrement = false;
      }
    }

    void
    advance(difference_type & offset)
    {
      myValue += offset;
    }

    difference_type
    distance_to(const ConstMultiIdxIterator< Integer> & other) const
    {
      return other.myValue - myValue;
    }

    bool
    equal(const ConstMultiIdxIterator & other) const
    {
      return myValue == other.myValue;
    }

    reference
    dereference() const
    {
      return myValue;
    }

    value_type myValue;
    const MultiIdxRange< Integer> & myRange;

    friend class ::boost::iterator_core_access;
  };

} // namespace utility_details

template< typename Integer>
  class MultiIdxRange : public ::boost::iterator_range<
      utility_detail::ConstMultiIdxIterator< Integer> >
  {
    typedef utility_detail::ConstMultiIdxIterator< Integer> iterator_t;
    typedef ::boost::iterator_range< iterator_t> base_t;

  public:
    MultiIdxRange(const MultiIdx< Integer> & first,
        const MultiIdx< Integer> & last) :
        base_t(iterator_t(first, *this), iterator_t(last, *this)), myBegin(
            first), myEnd(last)
    {
      SSLIB_ASSERT(myBegin != myEnd);
    }

    size_t
    dims() const
    {
      return myBegin.dims();
    }

  private:

    const MultiIdx< Integer> myBegin;
    const MultiIdx< Integer> myEnd;

#if SSLIB_USE_CPP11
    friend class iterator_t;
#else
    // Sadly can't use iterator_t typedef here as this behaviour is not supported
    // until C++11.
    // See e.g.: http://stackoverflow.com/questions/392120/why-cant-i-declare-a-friend-through-a-typedef
    friend class utility_detail::ConstMultiIdxIterator< Integer>;
#endif
  };

} // namespace utility
} // namespace spl

#ifdef _MSC_VER
#  pragma warning( pop )
#endif

#endif /* MULTI_IDX_RANGE_H */
