/*
 * Assert.h
 *
 *
 *  Created on: Feb 6, 2013
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_ASSERT_H
#define SCHEMER_ASSERT_H

// INCLUDES /////////////////////////////////////////////
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/version.hpp>

// DEFINES //////////////////////////////////////////////
#define SCHEMER_ASSERT(expr) BOOST_ASSERT(expr)
#define SCHEMER_STATIC_ASSERT(expr) BOOST_STATIC_ASSERT(expr)

#if (BOOST_VERSION / 100000) <= 1 && ((BOOST_VERSION / 100) % 1000) <= 45
  // No BOOST_ASSERT_MSG.  Was introduced in 1.46
#  define SCHEMER_ASSERT_MSG(expr, msg) BOOST_ASSERT(expr)
#else
#  define SCHEMER_ASSERT_MSG(expr, msg) BOOST_ASSERT_MSG(expr, msg)
#endif

#if (BOOST_VERSION / 100000) <= 1 && ((BOOST_VERSION / 100) % 1000) <= 47
  // No BOOST_STATIC_ASSERT_MSG.  Was introduced in 1.48
#  define SCHEMER_STATIC_ASSERT_MSG(expr, msg) BOOST_STATIC_ASSERT(expr)
#else
#  define SCHEMER_STATIC_ASSERT_MSG(expr, msg) BOOST_STATIC_ASSERT_MSG(expr, msg)
#endif


#endif /* SCHEMER_ASSERT_H */
