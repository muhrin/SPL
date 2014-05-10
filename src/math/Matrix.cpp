/*
 * Matrix.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/SSLibAssert.h"
#include "spl/math/Matrix.h"
#include "spl/utility/StableComparison.h"
#include "spl/utility/IndexingEnums.h"

#include <limits>

namespace spl {
namespace math {

using namespace arma;

namespace comp = utility::stable;

void
normalise(::arma::vec & vec)
{
  vec = vec / sqrt(::arma::dot(vec, vec));
}

::arma::vec
normaliseCopy(const ::arma::vec & vec)
{
  ::arma::vec copy(vec);
  normalise(copy);
  return copy;
}

bool
equals(const ::arma::mat & a, const ::arma::mat & b)
{
  return equals(a, b, 1e3 * ::std::numeric_limits< double>::epsilon());
}

bool
equals(const ::arma::mat & a, const ::arma::mat & b, const double tolerance)
{
  if(a.n_rows != b.n_rows || a.n_cols != b.n_cols)
    return false;

  const ::arma::mat diff(::arma::abs(a - b));
  return comp::eq(diff.max(), 0.0, tolerance);
}

bool
isZero(const ::arma::mat & mat)
{
  return isZero(mat, 1e3 * ::std::numeric_limits< double>::epsilon());
}

bool
isZero(const ::arma::mat & mat, const double tolerance)
{
  const ::arma::mat abs(::arma::abs(mat));
  return comp::eq(abs.max(), 0.0, tolerance);
}

bool
isSpannedBy(const ::arma::mat & superspace, const ::arma::mat & subspace)
{
  SSLIB_ASSERT(superspace.n_rows == subspace.n_rows);
  SSLIB_ASSERT(superspace.n_cols >= subspace.n_cols);

  // For the superspace to span the subspace there must be a non-zero
  // solution to the equation A * X = B
  bool isSpanned = false;
  ::arma::mat x;
  if(::arma::solve(x, superspace, subspace))
    isSpanned = !isZero(x);

  return isSpanned;
}

bool
isReal(const ::arma::cx_vec & vec)
{
  return isReal(vec, 1e3 * ::std::numeric_limits< double>::epsilon());
}

bool
isReal(const ::arma::cx_vec & vec, const double tolerance)
{
  const ::arma::vec im(::arma::imag(vec));
  return comp::eq(im.max(), 0.0, tolerance);
}

bool
isReal(const ::arma::cx_mat & mat)
{
  return isReal(mat, 1e3 * ::std::numeric_limits< double>::epsilon());
}

bool
isReal(const ::arma::cx_mat & mat, const double tolerance)
{
  const ::arma::vec im(::arma::imag(mat));
  return comp::eq(im.max(), 0.0, tolerance);
}

::arma::mat44
makeTranslation(const ::arma::vec3 & translation)
{
  using namespace utility::cart_coords_enum;
  ::arma::mat44 transform = ::arma::eye(4, 4);
  transform.col(3).rows(X, Z) = translation;
  return transform;
}

::arma::vec3
getTranslation(const mat44 & transform)
{
  using namespace utility::cart_coords_enum;
  return transform.col(3).rows(X, Z);
}

void
setTranslation(::arma::mat44 & transform, const vec3 & translation)
{
  using namespace utility::cart_coords_enum;
  transform.col(3).rows(X, Z) = translation;
}

void
inverse(::arma::mat44 & transform)
{
  using namespace utility::cart_coords_enum;
  transform.submat(0, 0, 2, 2) = ::arma::inv(transform.submat(0, 0, 2, 2));
  transform.col(3).rows(X, Z) = -transform.col(3).rows(X, Z);
}

::arma::mat44
inverseCopy(const ::arma::mat44 & transform)
{
  ::arma::mat44 copy = transform;
  inverse(copy);
  return copy;
}

::arma::mat33
getRotation(const ::arma::mat44 & transform)
{
  return transform.submat(0, 0, 2, 2);
}

void
setRotation(::arma::mat44 & transform, const vec4 & axisAngle)
{
  using namespace utility::cart_coords_enum;

  // Using Rodrigues' rotation formula
  // see: http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
  vec3 axis(axisAngle.rows(X, Z));
  normalise(axis);
  const double theta = axisAngle(3);

  mat33 crossProductMtx;
  crossProductMtx.zeros();
  crossProductMtx(0, 1) = -axis(Z);
  crossProductMtx(0, 2) = axis(Y);
  crossProductMtx(1, 2) = -axis(X);
  // Skew symmetric replication
  crossProductMtx(1, 0) = axis(Z);
  crossProductMtx(2, 0) = -axis(Y);
  crossProductMtx(2, 1) = axis(X);

  transform.submat(0, 0, 2, 2) = eye< mat>(3, 3) * cos(theta)
      + sin(theta) * crossProductMtx
      + (1.0 - cos(theta)) * kron(axis, axis.t());
}

void
transform(::arma::vec3 & vec, const ::arma::mat44 & transformMtx)
{
  using namespace utility::cart_coords_enum;

  ::arma::vec4 transformed;
  transformed.rows(X, Z) = vec;
  transformed(3) = 1.0;
  transformed = transformMtx * transformed;
  vec = transformed.rows(X, Z);
}

::arma::vec3
transformCopy(const ::arma::vec3 & vec, const ::arma::mat44 & transformMtx)
{
  ::arma::vec3 copy(vec);
  transform(copy, transformMtx);
  return copy;
}

}
}
