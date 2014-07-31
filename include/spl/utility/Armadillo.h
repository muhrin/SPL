/*
 * Armadillo.h
 *
 *
 *  Created on: Nov 1, 2013
 *      Author: Martin Uhrin
 */

#ifndef ARMADILLO_H
#define ARMADILLO_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL
#  include <CGAL/Point_2.h>
#  include <CGAL/Vector_2.h>
#endif

#include <boost/functional/hash_fwd.hpp>

#include <armadillo>

namespace spl {
namespace utility {

#ifdef SPL_USE_CGAL

template< typename K>
  CGAL::Point_2< K>
  toCgalPoint(const arma::vec2 & r)
  {
    return CGAL::Point_2< K>(r(0), r(1));
  }

template< typename K>
  CGAL::Vector_2< K>
  toCgalVec(const arma::vec2 & r)
  {
    return CGAL::Vector_2< K>(r(0), r(1));
  }

template< typename K>
  arma::vec2
  toArma(const CGAL::Vector_2< K> & v)
  {
    arma::vec2 armaVec;
    armaVec(0) = CGAL::to_double(v.x());
    armaVec(1) = CGAL::to_double(v.y());
    return armaVec;
  }

#endif

}
}

namespace arma {

std::size_t
hash_value(const vec & v);

template< typename T>
  std::istream &
  operator >>(std::istream &in, arma::Mat< T> & mat)
  {
    mat.quiet_load(in);
    return in;
  }

}

#endif /* ARMADILLO_H */
