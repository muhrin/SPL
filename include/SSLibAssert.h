/*
 * SSLibAssert.h
 *
 *  Created on: Sep 5, 2011
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_ASSERT_H
#define SSLIB_ASSERT_H

// INCLUDES ///////////////////////////////////////////////

#include <boost/assert.hpp>

// DEFINES ///////////////////////////////////////////////

#define SSLIB_ASSERT(expr) BOOST_ASSERT(expr)

#if (BOOST_VERSION / 100000) <= 1 && ((BOOST_VERSION / 100) % 1000) <= 45
  // No BOOST_ASSERT_MSG.  Was introduced in 1.46
#  define SSLIB_ASSERT_MSG(expr, msg) BOOST_ASSERT(expr)
#else
#  define SSLIB_ASSERT_MSG(expr, msg) BOOST_ASSERT_MSG(expr, msg)
#endif

#define SSLIB_PRECONDITION(expr) SSLIB_ASSERT(expr)
#define SSLIB_PRECONDITION_MSG(expr) SSLIB_ASSERT_MSG(expr)

#endif /* SSLIB_ASSERT_H */
