/*
 * SSLibTypes.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_TYPES_H
#define SSLIB_TYPES_H


// INCLUDES //////////////////////////////////////
#include <complex>
#include <climits>
#include <limits>
#include <memory>

#include <boost/static_assert.hpp>
#include <boost/utility.hpp>

namespace sstbx {

// If C++11 is available then use std::unique_ptr, otherwise
// std::auto_ptr is used.  This means that the user should taylor
// their code using the define if they want to use any specific
// behaviour
template <typename T>
struct UniquePtr : ::boost::noncopyable
{
#ifdef SSLIB_USE_CPP11
  typedef ::std::unique_ptr<T> Type;
#else
  typedef ::std::auto_ptr<T> Type;
#endif
private:
  UniquePtr() {}
};


#ifdef BOOST_STATIC_ASSERT_MSG
#  define SSLIB_STATIC_ASSERT_MSG( B, Msg ) BOOST_STATIC_ASSERT_MSG(B, Msg)
#else
#  define SSLIB_STATIC_ASSERT_MSG( B, Msg ) BOOST_STATIC_ASSERT( B )
#endif

// NUMERIC TYPES ///////////////////////////////////////

#if UCHAR_MAX >= 0xff
  //! unsigned 8 bit type
  typedef unsigned char u8;
  typedef          char s8;
#else
  #error "don't know how to typedef 'u8' on this system"
#endif

// NOTE:
// "signed char" is not the same as "char". 
// http://www.embedded.com/columns/programmingpointers/206107018
// http://en.wikipedia.org/wiki/C_variable_types_and_declarations


#if USHRT_MAX >= 0xffff
  //! unsigned 16 bit type  
  typedef unsigned short u16;
  typedef          short s16;
#else
  #error "don't know how to typedef 'u16' on this system"
#endif


#if   UINT_MAX  >= 0xffffffff
  typedef unsigned int  u32;
  typedef          int  s32;
#elif ULONG_MAX >= 0xffffffff
  typedef unsigned long u32;
  typedef          long s32;
#else
  #error "don't know how to typedef 'u32' on this system"
#endif


#if defined(SSLIB_64BIT_WORD)
  #if    ULONG_MAX >= 0xffffffffffffffff
    typedef unsigned long      u64;
    typedef          long      s64;
  #else
    #if ULLONG_MAX >= 0xffffffffffffffff
      typedef unsigned long long u64;
      typedef          long long s64;
    #else
      #error "don't know how to typedef 'u64' on this system"
    #endif
  #endif
#endif



struct type_check
{
private:
SSLIB_STATIC_ASSERT_MSG(sizeof(u8) == 1, "Error, type u8 has unsupported size" );
SSLIB_STATIC_ASSERT_MSG(sizeof(s8) == 1, "Error, type s8 has unsupported size");

SSLIB_STATIC_ASSERT_MSG(sizeof(u16) == 2, "Error, type u16 has unsupported size");
SSLIB_STATIC_ASSERT_MSG(sizeof(s16) == 2, "Error, type s16 has unsupported size");

SSLIB_STATIC_ASSERT_MSG(sizeof(u32) == 4, "Error, type u32 has unsupported size");
SSLIB_STATIC_ASSERT_MSG(sizeof(s32) == 4, "Error, type s32 has unsupported size");

#if defined(SSLIB_64BIT_WORD)
BOOST_STATIC_ASSERT_MSG(sizeof(u64) == 8, "Error, type u64 has unsupported size");
BOOST_STATIC_ASSERT_MSG(sizeof(s64) == 8, "Error, type s64 has unsupported size");
#endif

SSLIB_STATIC_ASSERT_MSG(sizeof(float)  == 4, "Error, type float has unsupported size");
SSLIB_STATIC_ASSERT_MSG(sizeof(double) == 8, "Error, type double has unsupported size");

SSLIB_STATIC_ASSERT_MSG(sizeof(std::complex<float>)  == 8,  "Error, type complex float has unsupported size");
SSLIB_STATIC_ASSERT_MSG(sizeof(std::complex<double>) == 16, "Error, type complex double has unsupported size");
  
};
}


#endif /* SSLIB_TYPES_H */
