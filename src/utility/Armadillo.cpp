/*
 * Armadillo.cpp
 *
 *  Created on: Nov 1, 2013
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/utility/Armadillo.h"

#include <boost/functional/hash.hpp>

namespace arma {

std::size_t hash_value(const vec & v)
{
  return ::boost::hash_range(v.begin(), v.end());
}

}
