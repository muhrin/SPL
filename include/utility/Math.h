/*
 * Math.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MATH_H
#define MATH_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx {
namespace utility {
namespace math {

/**
/* Euclid's algorithm for computing the greatest common divisor.
/**/
inline unsigned int greatestCommonDivisor(const unsigned int a, const unsigned int b)
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
  } while(remainder != 0);

  return larger;
}

inline unsigned int leastCommonMultiple(const unsigned a, const unsigned b)
{
  return a / greatestCommonDivisor(a, b) * b;
}

}
}
}

#endif /* MATH_H */
