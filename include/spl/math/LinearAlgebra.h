/*
 * LinearAlgebra.h
 *
 *  Created on: Jul 26, 2013
 *      Author: Martin Uhrin
 */

#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <armadillo>

#ifdef SPL_USE_CGAL
#include <CGAL/Point_3.h>
#endif // SPL_USE_CGAL

// DEFINITION ///////////////////////

namespace spl {
namespace math {

#ifdef SPL_USE_CGAL

template <typename Kernel>
inline ::arma::vec3 cgalToArma(const ::CGAL::Point_3<Kernel> & point)
{
  ::arma::vec3 ret;
  for(int i = 0; i < 3; ++i)
    ret[i] = CGAL::to_double(point[i]);
  return ret;
}

template <typename Kernel>
inline ::CGAL::Point_3<Kernel> armaToCgal(const ::arma::vec3 & point)
{
  return ::CGAL::Point_3<Kernel>(point[0], point[1], point[2]);
}

#endif // SPL_USE_CGAL

}
}


#endif /* LINEAR_ALGEBRA_H */
