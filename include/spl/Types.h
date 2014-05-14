/*
 * SSLibTypes.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_TYPES_H
#define SSLIB_TYPES_H


// INCLUDES //////////////////////////////////////
#include <memory>
#include <string>

#include <boost/noncopyable.hpp>

namespace spl {
namespace utility {
template <typename T>
class OrderedPair;
}

// If C++11 is available then use std::unique_ptr, otherwise
// std::auto_ptr is used.  This means that the user should tailor
// their code using the define if they want to use any specific
// behaviour
template <typename T>
struct UniquePtr : boost::noncopyable
{
#ifdef SSLIB_USE_CPP11
  typedef std::unique_ptr<T> Type;
#else
  typedef std::auto_ptr<T> Type;
#endif
private:
  UniquePtr() {}
};

template <typename T>
typename UniquePtr<T>::Type makeUniquePtr(T * const ptr)
{
  return typename UniquePtr<T>::Type(ptr);
}

// Sometimes it's not possible to use the above struct-typedef method,
// for example in cases where ADL should be used.  In which case this
// macro can be used for the pointer type instead.  It's less safe -
// but it works.
#ifdef SSLIB_USE_CPP11
#  define SSLIB_UNIQUE_PTR(T) std::unique_ptr<T>
#else
#  define SSLIB_UNIQUE_PTR(T) std::auto_ptr<T>
#endif

typedef spl::utility::OrderedPair< std::string> SpeciesPair;

}


#endif /* SSLIB_TYPES_H */
