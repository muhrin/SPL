/*
 * MultiIterator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MULTI_ITERATOR_H
#define MULTI_ITERATOR_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <boost/iterator/iterator_facade.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {


template <typename Integer>
class MultiIterator : public ::boost::iterator_facade<
  MultiIterator<Integer>,
  const MultiIdx<Integer>,
  ::boost::random_access_traversal_tag,
  const MultiIdx<Integer> &,
  MultiIdx<Integer>
>
{
  typedef ::boost::iterator_facade<
    ConstMultiIdxIterator<Integer>,
    const MultiIdx<Integer>,
    ::boost::random_access_traversal_tag,
    const MultiIdx<Integer> &,
    MultiIdx<Integer>
  > base_t;

public:
  typedef typename base_t::value_type value_type;
  typedef typename base_t::difference_type difference_type;
  typedef typename base_t::reference reference;

  ConstMultiIdxIterator(value_type x, const MultiIdxRange<Integer> & range) : myValue(x), myRange(range) {}

private:

  void increment()
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

  void decrement()
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

  void advance(difference_type & offset)
  {
    myValue += offset;
  }

  difference_type distance_to(const ConstMultiIdxIterator<Integer> & other) const
  {
    return other.myValue - myValue;
  }

  bool equal(const ConstMultiIdxIterator & other) const
  {
    return myValue == other.myValue;
  }

  reference dereference() const
  {
    return myValue;
  }

  value_type                      myValue;
  const MultiIdxRange<Integer> &  myRange;


  friend class ::boost::iterator_core_access;
};


} // namespace utility
} // namespace spl


#endif /* MULTI_ITERATOR_H */
