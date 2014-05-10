/*
 * Random.h
 *
 *  Created on: Aug 22, 2011
 *      Author: Martin Uhrin
 */

#ifndef RANDOM_DETAIL_H
#define RANDOM_DETAIL_H

// INCLUDES ///////////////////////////////////////
#include <stdlib.h>
#include <time.h>

#include <boost/version.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#if BOOST_VERSION / 100000 <= 1 && BOOST_VERSION / 100 % 1000 <= 42
#  define SSLIB_USE_BOOST_OLD_RANDOM
#  include <boost/random/uniform_int.hpp>
#  include <boost/random/uniform_real.hpp>
#  include <boost/random/variate_generator.hpp>
#else
#  include <boost/random/uniform_int_distribution.hpp>
#  include <boost/random/uniform_real_distribution.hpp>
#endif
#include <boost/type_traits/is_integral.hpp>

#include "spl/SSLibAssert.h"

// FORWARD DECLARES ////////////////////////////////

// DEFINES ////////////////////////////////////////

// FUNCTIONS ////////////////////////////////////////

namespace spl {
namespace math {
namespace detail {

template< typename T, bool isIntegral = boost::is_integral< T>::value>
  struct Rand;

extern boost::mt19937 mt19937;

// Specialisations
template< typename T>
  struct Rand< T, true>
  {
    static T
    getUniform(const T to)
    {
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
      const boost::uniform_int< T> dist(0, to);
      boost::variate_generator<boost::mt19937&, boost::uniform_int< T> > gen(mt19937, dist);
      return gen();
#else
      const boost::random::uniform_int_distribution< T> dist(0, to);
      return dist(mt19937);
#endif
    }
    static T
    getUniform(const T from, const T to)
    {
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
      const boost::uniform_int< T> dist(from, to);
      boost::variate_generator<boost::mt19937&, boost::uniform_int< T> > gen(mt19937, dist);
      return gen();
#else
      const boost::random::uniform_int_distribution< T> dist(from, to);
      return dist(mt19937);
#endif
    }
  };

template< typename T>
  struct Rand< T, false>
  {
    static boost::normal_distribution< T> normal;
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
    static const boost::uniform_real< T> uniform;
    static boost::variate_generator< boost::mt19937 &, boost::uniform_real< T> > uniformGenerator;
    static boost::variate_generator< boost::mt19937 &, boost::normal_distribution< T> > normalGenerator;
#else
    static const boost::random::uniform_real_distribution< T> uniform;
#endif

    static T
    getUniform()
    {
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
      return uniformGenerator();
#else
      return uniform(mt19937);
#endif
    }
    static T
    getUniform(const T to)
    {
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
      return uniformGenerator() * to;
#else
      return uniform(mt19937) * to;
#endif
    }
    static T
    getUniform(const T from, const T to)
    {
      return getUniform(to - from) + from;
    }
    static T
    getNormal()
    {
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
      return normalGenerator();
#else
      return normal(mt19937);
#endif
    }
    static T
    getNormal(const T mean, const T variance)
    {
      boost::normal_distribution< T> normal(mean, variance);
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
      boost::variate_generator<boost::mt19937&, boost::normal_distribution< T> >
      normalGen(mt19937, normal);
      normalGen();
#else
      return normal(mt19937);
#endif
    }
  };

template< typename T>
  boost::normal_distribution< T> Rand< T, false>::normal(0.0, 1.0);
#ifdef SSLIB_USE_BOOST_OLD_RANDOM
template< typename T>
  const boost::uniform_real<T> Rand< T, false>::uniform(0.0, 1.0);
template< typename T>
  boost::variate_generator< boost::mt19937 &, boost::uniform_real< T> >
  Rand< T, false>::uniformGenerator(mt19937, uniform);
template< typename T>
  boost::variate_generator< boost::mt19937 &, boost::normal_distribution< T> >
  Rand< T, false>::normalGenerator(mt19937, normal);
#else
template< typename T>
  const boost::random::uniform_real_distribution< T> Rand< T, false>::uniform(
      0.0, 1.0);
#endif

} // namespace detail

inline void
seed(const unsigned int randSeed)
{
  std::srand(randSeed);
  detail::mt19937.seed(randSeed);
}

inline void
seed()
{
  std::srand(static_cast< unsigned int>(time(NULL)));
  detail::mt19937.seed(static_cast< unsigned int>(time(NULL)));
}

template< typename T>
  inline T
  randu()
  {
    return detail::Rand< T>::getUniform();
  }

template< typename T>
  inline T
  randu(const T to)
  {
    return detail::Rand< T>::getUniform(to);
  }

template< typename T>
  inline T
  randu(const T from, const T to)
  {
    return detail::Rand< T>::getUniform(from, to);
  }

template< typename T>
  inline T
  randn()
  {
    return detail::Rand< T>::getNormal();
  }
template< typename T>
  inline T
  randn(const T mean, const T variance)
  {
    return detail::Rand< T>::getNormal(mean, variance);
  }

}
}

#endif /* RANDOM_DETAIL_H */
