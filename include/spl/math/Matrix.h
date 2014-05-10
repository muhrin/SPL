/*
 * Matrix.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MATRIX_H
#define MATRIX_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <armadillo>

// DEFINITION ///////////////////////

namespace spl {
namespace math {

void
normalise(::arma::vec & vec);
::arma::vec
normaliseCopy(const ::arma::vec & vec);

bool
equals(const ::arma::mat & a, const ::arma::mat & b);
bool
equals(const ::arma::mat & a, const ::arma::mat & b, const double tolerance);

bool
isZero(const ::arma::mat & mat);
bool
isZero(const ::arma::mat & mat, const double tolerance);

bool
isSpannedBy(const ::arma::mat & superspace, const ::arma::mat & subspace);

bool
isReal(const ::arma::cx_vec & vec);
bool
isReal(const ::arma::cx_vec & vec, const double tolerance);

bool
isReal(const ::arma::cx_mat & mat);
bool
isReal(const ::arma::cx_mat & mat, const double tolerance);


::arma::mat44
makeTranslation(const ::arma::vec3 & translation);
::arma::vec3
getTranslation(const ::arma::mat44 & transform);
void
setTranslation(::arma::mat44 & transform, const ::arma::vec3 & translation);

void
inverse(::arma::mat44 & transform);
::arma::mat44
inverseCopy(const ::arma::mat44 & transform);

::arma::mat33
getRotation(const ::arma::mat44 & transform);
void
setRotation(::arma::mat44 & transform, const ::arma::vec4 & axisAngle);

void
transform(::arma::vec3 & vec, const ::arma::mat44 & transformMtx);
::arma::vec3
transformCopy(const ::arma::vec3 & vec, const ::arma::mat44 & transformMtx);

}
}

#endif /* MATRIX_H */
