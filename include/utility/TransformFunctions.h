/*
 * TransformFunctions.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef TRANSFORM_FUNCTIONS_H
#define TRANSFORM_FUNCTIONS_H

// INCLUDES /////////////////////////////////////////////
#include <functional>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace utility {

// Return the first value of a pair
template <typename T1, typename T2>
class TakeFirst : public ::std::unary_function< const ::std::pair<T1, T2> &, T1>
{
public:
  T1 operator() (const ::std::pair<T1, T2> & pair) const
  {
    return pair.first;
  }
};


// Return the second value of a pair
template <typename T1, typename T2>
class TakeSecond : public ::std::unary_function< const ::std::pair<T1, T2> &, T2>
{
public:
  T2 operator() (const ::std::pair<T1, T2> & pair) const
  {
    return pair.second;
  }
};

}
}

#endif /* TRANSFORM_FUNCTIONS_H */
