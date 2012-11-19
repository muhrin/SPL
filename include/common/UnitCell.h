/*
 * UnitCell.h
 *
 *  Created on: Sep 3, 2012
 *      Author: Martin Uhrin
 */

#ifndef UNIT_CELL_H
#define UNIT_CELL_H

// INCLUDES ///////////////////////////////////
#include <armadillo>

#include <boost/noncopyable.hpp>

#include "SSLibAssert.h"
#include "common/Types.h"
#include "utility/StableComparison.h"

namespace sstbx {
namespace common {

// FORWARD DECLARES ///////////////////////////
class Structure;

class UnitCell : private ::boost::noncopyable
{
public:

  struct LatticeSystem
  {
    enum Value
    {
      TRICLINIC,
      MONOCLINIC,
      ORTHORHOMBIC,
      RHOMBOHEDRAL,
      TETRAGONAL,
      HEXAGONAL,
      CUBIC
    };
  };

  UnitCell(
    const double a, const double b, const double c,
    const double alpha, const double beta, const double gamma);
  explicit UnitCell(const double (&latticeParams)[6]);
  explicit UnitCell(const ::arma::mat33 & orthoMatrix);
  UnitCell(const UnitCell & toCopy);

  UnitCellPtr clone() const;

  const double (&getLatticeParams() const)[6];

  inline ::arma::vec3 getAVec() const
  {
    return myOrthoMtx.col(0);
  }

  inline ::arma::vec3 getBVec() const
  {
    return myOrthoMtx.col(1);
  }

  inline ::arma::vec3 getCVec() const
  {
    return myOrthoMtx.col(2);
  }

  double getLongestCellVectorLength() const;

  const ::arma::mat33 & getOrthoMtx() const;
  const void setOrthoMtx(const ::arma::mat33 & orthoMtx);

  const ::arma::mat33 & getFracMtx() const;

	double getVolume() const;
	double setVolume(const double volume);
  double getNormVolume() const;

  ::arma::vec3 getLongestDiagonal() const;

  LatticeSystem::Value getLatticeSystem(const double tolerance = utility::StableComp::STABLE_COMP_TOL) const;

  inline ::arma::vec3 fracToCart(const ::arma::vec3 & frac) const
  {
    return myOrthoMtx * frac;
  }

  inline ::arma::vec3 & fracToCartInplace(::arma::vec3 & frac) const
  {
	  frac = myOrthoMtx * frac;
	  return frac;
  }

  inline ::arma::mat & fracsToCartInplace(::arma::mat & fracs) const
  {
    SSLIB_ASSERT(fracs.n_rows == 3);

    fracs = myOrthoMtx * fracs;
    return fracs;
  }

  inline ::arma::vec3 & fracWrapToCartInplace(::arma::vec3 & frac) const
  {
    frac -= ::arma::floor(frac);  // Wrap
    frac = myOrthoMtx * frac;     // Convert to cartesian
    return frac;
  }

  inline ::arma::mat & fracsWrapToCartInplace(::arma::mat & fracs) const
  {
    fracs -= ::arma::floor(fracs);  // Wrap
    fracs = myOrthoMtx * fracs;     // Convert to cartesian
    return fracs;
  }

  inline ::arma::vec3 cartToFrac(const ::arma::vec3 & cart) const
  {
    ::arma::vec3 toFrac(cart);
    return cartToFracInplace(toFrac);
  }

  inline ::arma::vec3 & cartToFracInplace(::arma::vec3 & cart) const
  {
    cart = myFracMtx * cart;
    return cart;
  }

  inline ::arma::mat & cartsToFracInplace(::arma::mat & carts) const
  {
    SSLIB_ASSERT(carts.n_rows == 3);

	  carts = myFracMtx * carts;
	  return carts;
  }

  ::arma::vec3 wrapVec(const ::arma::vec3 & cart) const;
  ::arma::vec3 & wrapVecInplace(::arma::vec3 & cart) const;

  inline ::arma::mat & wrapVecsInplace(::arma::mat & carts) const
  {
    SSLIB_ASSERT(carts.n_rows == 3);

    carts = myFracMtx * carts;     // cart to frac
    carts -= ::arma::floor(carts); // wrap
    carts = myOrthoMtx * carts;    // frac to cart.  Simple.
    return carts;
  }

  ::arma::vec3 wrapVecFrac(const ::arma::vec3 & frac) const;
  ::arma::vec3 & wrapVecFracInplace(::arma::vec3 & frac) const;

  inline ::arma::mat & wrapVecsFracInplace(::arma::mat & fracs) const
  {
    SSLIB_ASSERT(fracs.n_rows == 3);

    fracs -= ::arma::floor(fracs);
    return fracs;
  }

  ::arma::vec3 randomPoint() const;

  bool niggliReduce();

private:

  /** Initialise the unit cell from lattice parameters */
	void init(
		const double a, const double b, const double c,
		const double alpha, const double beta, const double gamma);

	/** Initialise the unit cell from an orthogonalisation matrix */
  void init(const ::arma::mat33 & orthoMtx);
	void initOrthoAndFracMatrices();
	void initLatticeParams();
	void initRest();

  void setStructure(const Structure * const structure);

  const Structure * myStructure;

	/** The unit cell matrix where columns represent basis vectors */
  ::arma::mat33 myOrthoMtx;

	/** The inverse of the orthogonalisation matrix */
  ::arma::mat33 myFracMtx;

	double	myLatticeParams[6];

	double	myVolume;

  friend class Structure;
};

}
}

#endif /* UNIT_CELL_H */
