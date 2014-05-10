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

namespace spl {
namespace utility {

// Return the first value of a pair
template <typename T>
class TakeFirst : public ::std::unary_function< T &, typename T::first_type>
{
public:
  typename T::first_type operator() (T & pair) const
  { return pair.first; }
};


// Return the dereferenced first value of a pair
template <typename T, typename Reference>
class TakeFirstDereferenced : public ::std::unary_function< T &, Reference>
{
public:
  Reference operator() (T & pair) const
  { return *pair.first; }
};


// Return the second value of a pair
template <typename T>
class TakeSecond : public ::std::unary_function< T &, typename T::second_type>
{
public:
  typename T::second_type operator() (T & pair) const
  { return pair.second; }
};

// Return the dereferenced second value of a pair
template <typename T, typename Reference>
class TakeSecondDereferenced : public ::std::unary_function< T &, Reference>
{
public:
  Reference operator() (T & pair) const
  { return *pair.second; }
};

template <typename T>
class AddressOf : public ::std::unary_function< T &, T *>
{
public:
  T * operator() ( T & x) const
  { return &x; }
};

}
}

#endif /* TRANSFORM_FUNCTIONS_H */
