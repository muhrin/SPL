/*
 * FloatRnage.h
 *
 *
 *  Created on: Oct 7, 2012
 *      Author: Martin Uhrin
 */

#ifndef FLOAT_RANGE_H
#define FLOAT_RANGE_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace utility {

namespace utility_detail {

// FloatIteratorWithStep provides the ability to 'move' in
// a number of steps specified at construction time.
//
template<typename Float>
class FloatIteratorWithStep
    : public boost::iterator_facade<
                FloatIteratorWithStep<Float>,
                Float,
                boost::random_access_traversal_tag,
                Float,
                std::ptrdiff_t
            >
{
    typedef boost::iterator_facade<
                FloatIteratorWithStep<Float>,
                Float,
                boost::random_access_traversal_tag,
                Float,
                std::ptrdiff_t
            > base_t;
public:
    typedef typename base_t::value_type value_type;
    typedef typename base_t::difference_type difference_type;
    typedef typename base_t::reference reference;

    FloatIteratorWithStep(value_type first, difference_type step, value_type stepSize)
        : myFirst(first)
        , myStep(step)
        , myStepSize(step_size)
    {
    }

private:
    void increment()
    {
        ++myStep;
    }

    void decrement()
    {
        --myStep;
    }

    void advance(difference_type offset)
    {
        myStep += offset;
    }

    difference_type distance_to(const FloatIteratorWithStep & other) const
    {
        return other.myStep - myStep;
    }

    bool equal(const FloatIteratorWithStep & other) const
    {
        return myStep == other.myStep;
    }

    reference dereference() const
    {
        return myFirst + (myStep * myStepSize);
    }

    friend class ::boost::iterator_core_access;
    value_type myFirst;
    value_type myStep;
    difference_type myStepSize;
};

} // namespace utility_detail

template <typename Float>
class StridedFloatRange : public ::boost::iterator_range<utility_detail::FloatIteratorWithStep<Float> >
{
  typedef utility_detail::FloatIteratorWithStep<Float> iterator_t;
  typedef iterator_range<iterator_t> base_t;

public:

  template <typename Iterator>
  StridedFloatRange(Iterator first, Iterator last):
    base_t(first, last)
  {}
};

template <typename Float, typename StepSize>
StridedFloatRange<Float>
FloatRange(Float first, StepSize stepSize, size_t numSteps)
{
  SSLIB_ASSERT(stepSize != 0);

  typedef typename utility_detail::FloatIteratorWithStep<Float> iterator_t;

  return StridedFloatIterator<Float>(
    iterator_t(first, 0, stepSize),
    iterator_t(first, numSteps, stepSize)
  );
}

}
}

#endif /* FLOAT_RANGE_H */

