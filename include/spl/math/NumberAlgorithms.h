/*
 * Math.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef NUMBER_ALGORITHMS_H
#define NUMBER_ALGORITHMS_H

// INCLUDES /////////////////////////////////////////////
#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace math {

// Euclid's algorithm for computing the greatest common divisor.
inline unsigned int
greatestCommonDivisor(const unsigned int a, const unsigned int b)
{
  unsigned int larger;
  unsigned int smaller;

  if(a < b)
  {
    smaller = a;
    larger = b;
  }
  else
  {
    smaller = b;
    larger = a;
  }

  unsigned int remainder;
  do
  {
    remainder = larger % smaller;
    larger = smaller;
    smaller = remainder;
  }
  while(remainder != 0);

  return larger;
}

inline unsigned int
greatestCommonDivisor(const ::std::vector< unsigned int> numbers)
{
  if(numbers.empty())
    return 0;

  unsigned int result = numbers[0];
  for(size_t i = 0; i < numbers.size(); ++i)
    result = greatestCommonDivisor(result, numbers[i]);
  return result;
}

inline unsigned int
leastCommonMultiple(const unsigned a, const unsigned b)
{
  return a / greatestCommonDivisor(a, b) * b;
}

template< typename T>
  double
  sgn(const T val)
  {
    return (T(0) < val) - (val < T(0));
  }

template< typename T>
  T
  roundAwayFromZero(const T val)
  {
    return sgn(val) * std::ceil(std::fabs(val));
  }

}
}

#endif /* NUMBER_ALGORITHMS_H */
