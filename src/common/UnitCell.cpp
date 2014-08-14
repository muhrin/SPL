/*
 * UnitCell.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/common/UnitCell.h"

#include <boost/assert.hpp>

#include "spl/SSLibAssert.h"
#include "spl/common/Constants.h"
#include "spl/common/Structure.h"
#include "spl/utility/IndexingEnums.h"

namespace spl {
namespace common {

UnitCell::UnitCell()
{
  init(1.0, 1.0, 1.0, 90.0, 90.0, 90.0);
}

UnitCell::UnitCell(const double a, const double b, const double c,
    const double alpha, const double beta, const double gamma)
{
  init(a, b, c, alpha, beta, gamma);
}

UnitCell::UnitCell(const arma::mat33 & orthoMatrix)
{
  init(orthoMatrix);
}

UnitCell::UnitCell(const double (&latticeParams)[6])
{
  using namespace utility::cell_params_enum;

  init(latticeParams[A], latticeParams[B], latticeParams[C],
      latticeParams[ALPHA], latticeParams[BETA], latticeParams[GAMMA]);
}

UnitCell::UnitCell(const UnitCell & toCopy) :
    myOrthoMtx(toCopy.myOrthoMtx), myFracMtx(toCopy.myFracMtx), myVolume(
        toCopy.myVolume)
{
  std::copy(toCopy.myLatticeParams, toCopy.myLatticeParams + 6,
      myLatticeParams);
}

UnitCell::~UnitCell()
{
  BOOST_FOREACH(UnitCellListener * const l, myListeners)
  {
    l->onUnitCellDestroyed();
  }
}

UnitCell &
UnitCell::operator =(const UnitCell & rhs)
{
  myOrthoMtx = rhs.myOrthoMtx;
  myFracMtx = rhs.myFracMtx;
  myVolume = rhs.myVolume;
  std::copy(rhs.myLatticeParams, rhs.myLatticeParams + 6, myLatticeParams);

  return *this;
}

bool
UnitCell::operator ==(const UnitCell & rhs) const
{
  for(size_t i = 0; i < 6; ++i)
  {
    if(myLatticeParams[i] != rhs.myLatticeParams[i])
      return false;
  }
  return true;
}

UnitCellPtr
UnitCell::clone() const
{
  return UnitCellPtr(new UnitCell(*this));
}

const double (&
    UnitCell::getLatticeParams() const)[6]
    {
      return myLatticeParams;
    }

    void
    UnitCell::setLatticeParams(const double (&params)[6])
    {
      using namespace utility::cell_params_enum;

      init(params[A], params[B], params[C], params[ALPHA], params[BETA],
          params[GAMMA]);

      sendUnitCellChangedMsg();
    }

    double
    UnitCell::getLongestCellVectorLength() const
    {
      using namespace utility::cell_params_enum;

      return std::max(myLatticeParams[A],
          std::max(myLatticeParams[B], myLatticeParams[C]));
    }

    const arma::mat33 &
    UnitCell::getOrthoMtx() const
    {
      return myOrthoMtx;
    }

    bool
    UnitCell::setOrthoMtx(const arma::mat33 & orthoMtx)
    {
      if(!init(orthoMtx))
        return false;

      sendUnitCellChangedMsg();

      return true;
    }

    const arma::mat33 &
    UnitCell::getFracMtx() const
    {
      return myFracMtx;
    }

    double
    UnitCell::getVolume() const
    {
      return myVolume;
    }

    double
    UnitCell::setVolume(const double volume)
    {
      SSLIB_ASSERT(volume > 0);

      const double oldVol = getVolume();

      const double scale = std::pow(volume / oldVol, 1.0 / 3.0);
      init(scale * myOrthoMtx);

      sendUnitCellVolChangedMsg(oldVol);

      return scale;
    }

    double
    UnitCell::getNormVolume() const
    {
      using namespace utility::cell_params_enum;

      // First normalise the lattice vectors
      arma::vec3 a(myOrthoMtx.col(A));
      arma::vec3 b(myOrthoMtx.col(B));
      arma::vec3 c(myOrthoMtx.col(C));

      a /= myLatticeParams[A];
      b /= myLatticeParams[B];
      c /= myLatticeParams[C];

      // Now calculate abs value of the the triple product
      return std::abs(dot(arma::cross(a, b), c));
    }

    arma::vec3
    UnitCell::getLongestDiagonal() const
    {
      const arma::vec3 A = getAVec();
      const arma::vec3 B = getBVec();
      const arma::vec3 C = getCVec();

      // This doesn't work:
      //arma::vec3 diag = A;
      //if(arma::dot(A, B) > 0.0)
      //  diag += B;
      //else
      //  diag -= B;

      //if(arma::dot(diag, C) > 0.0)
      //  diag += C;
      //else
      //  diag -= C;

      arma::vec3 longest = A + B + C;
      double maxLengthSq = arma::dot(longest, longest);

      arma::vec3 combination = A + B - C;
      double lengthSq = arma::dot(combination, combination);
      if(lengthSq > maxLengthSq)
      {
        longest = combination;
        maxLengthSq = lengthSq;
      }

      combination = A - B + C;
      lengthSq = arma::dot(combination, combination);
      if(lengthSq > maxLengthSq)
      {
        longest = combination;
        maxLengthSq = lengthSq;
      }

      combination = A - B - C;
      lengthSq = arma::dot(combination, combination);
      if(lengthSq > maxLengthSq)
      {
        longest = combination;
        maxLengthSq = lengthSq;
      }

      return longest;
    }

    UnitCell::LatticeSystem::Value
    UnitCell::getLatticeSystem(const double tolerance) const
    {
      using namespace utility::cell_params_enum;
      namespace comp = utility::stable;

      if(comp::eq(myLatticeParams[ALPHA], myLatticeParams[BETA], tolerance)
          && comp::eq(myLatticeParams[BETA], myLatticeParams[GAMMA], tolerance))
      {
        if(comp::eq(myLatticeParams[ALPHA], 90.0, tolerance))
        {
          if(comp::eq(myLatticeParams[A], myLatticeParams[B], tolerance))
          {
            if(comp::eq(myLatticeParams[A], myLatticeParams[C], tolerance))
            {
              return LatticeSystem::CUBIC;
            }
            else
            {
              return LatticeSystem::TETRAGONAL;
            }
          } // if(a == b)
          else if(comp::eq(myLatticeParams[A], myLatticeParams[C], tolerance))
          {
            return LatticeSystem::TETRAGONAL;
          } // if(a == c)
          else
          {
            return LatticeSystem::ORTHORHOMBIC;
          } // if(a == b), elseif(a == c), else
        } // if(alpha == 90)
        else
          return LatticeSystem::RHOMBOHEDRAL;
      } // if(alpha == beta == gamma)
      else
      {
        if(comp::eq(myLatticeParams[ALPHA], 90.0, tolerance))
        {
          if(comp::eq(myLatticeParams[BETA], 90.0, tolerance)
              || comp::eq(myLatticeParams[GAMMA], 90.0, tolerance))
          {
            return LatticeSystem::MONOCLINIC;
          }
          else
          {
            // Technically, this is triclinic but it probably indicates that there is a better
            // centering for the system
            return LatticeSystem::TRICLINIC;
          }
        } // if(alpha == 90)
        else if(comp::eq(myLatticeParams[BETA], 90.0, tolerance))
        {
          if(comp::eq(myLatticeParams[GAMMA], 90.0, tolerance))
          {
            return LatticeSystem::MONOCLINIC;
          }
          else
          {
            // Technically, this is triclinic but it probably indicates that there is a better
            // centering for the system
            return LatticeSystem::TRICLINIC;
          }
        } // if(alpha == 90), else if(beta == 90.0)
        else if(comp::eq(myLatticeParams[GAMMA], 90.0, tolerance))
        {
          return LatticeSystem::MONOCLINIC;
        } // if(alpha == 90), else if(beta == 90), else if(gamma == 90)
        else
        {
          return LatticeSystem::TRICLINIC;
        }
      }

      return LatticeSystem::TRICLINIC;
    }

    arma::vec3
    UnitCell::wrapVec(const arma::vec3 & cart) const
    {
      arma::vec3 toWrap(cart);
      cartToFracInplace(toWrap); // Fractionalise it
      wrapVecFracInplace(toWrap); // Wrap it
      return fracToCartInplace(toWrap); // Orthogonalise it back
    }

    arma::vec3 &
    UnitCell::wrapVecInplace(arma::vec3 & cart) const
    {
      cartToFracInplace(cart);
      wrapVecFracInplace(cart);
      fracToCartInplace(cart);
      return cart;
    }

    arma::vec3
    UnitCell::wrapVecFrac(const arma::vec3 & frac) const
    {
      arma::vec3 toWrap(frac);
      wrapVecFracInplace(toWrap);
      return toWrap;
    }

    arma::vec3 &
    UnitCell::wrapVecFracInplace(arma::vec3 & frac) const
    {
      frac -= arma::floor(frac);
      return frac;
    }

    arma::vec3
    UnitCell::randomPoint() const
    {
      arma::vec3 rand;
      rand.randu();
      return fracToCartInplace(rand);
    }

    bool
    UnitCell::niggliReduce()
    {
      // Borrowed from David Lonie's XtalOpt here:
      // http://github.com/dlonie/XtalComp/blob/master/xtalcomp.cpp#L1538
      //
      // Implements the niggli reduction algorithm detailed in:
      // Grosse-Kunstleve RW, Sauter NK, Adams PD. Numerically stable
      // algorithms for the computation of reduced unit cells. Acta
      // Crystallographica Section A Foundations of
      // Crystallography. 2003;60(1):1-6.

      using namespace utility::cell_params_enum;
      using namespace spl::utility;
      using std::fabs;
      using std::cos;

      // Set maximum number of iterations
      const unsigned int iterations = 1000;

      // For sanity checks:
      const double origVolume = getVolume();

      // Cache the current fractional coordinates for later.
      //QList<Eigen::Vector3d> fcoords = currentFractionalCoords();

      // Get cell parameters in storage units, convert deg->rad
      double a = myLatticeParams[A];
      double b = myLatticeParams[B];
      double c = myLatticeParams[C];
      double alpha = myLatticeParams[ALPHA] * constants::DEG_TO_RAD;
      double beta = myLatticeParams[BETA] * constants::DEG_TO_RAD;
      double gamma = myLatticeParams[GAMMA] * constants::DEG_TO_RAD;

      // Compute characteristic (step 0)
      double A = a * a;
      double B = b * b;
      double C = c * c;
      double xi = 2 * b * c * cos(alpha);
      double eta = 2 * a * c * cos(beta);
      double zeta = 2 * a * b * cos(gamma);

      // Return value
      bool ret = false;

      // comparison tolerance
      const double tol = stable::STABLE_COMP_TOL
          * std::pow(origVolume, 1.0 / 3.0);

      // Initialize change of basis matrices:
      //
      // Although the reduction algorithm produces quantities directly
      // relatible to a,b,c,alpha,beta,gamma, we will calculate a change
      // of basis matrix to use instead, and discard A, B, C, xi, eta,
      // zeta. By multiplying the change of basis matrix against the
      // current cell matrix, we avoid the problem of handling the
      // orientation matrix already present in the cell. The inverse of
      // this matrix can also be used later to convert the atomic
      // positions.
      // tmpMat is used to build other matrices
      arma::mat33 tmpMat;

      // Cache static matrices:

      // Swap x,y (Used in Step 1). Negatives ensure proper sign of final
      // determinant.
      tmpMat << 0 << -1 << 0 << arma::endr << -1 << 0 << 0 << arma::endr << 0
          << 0 << -1 << arma::endr;

      const arma::mat33 C1(tmpMat);
      // Swap y,z (Used in Step 2). Negatives ensure proper sign of final
      // determinant
      tmpMat << -1 << 0 << 0 << arma::endr << 0 << 0 << -1 << arma::endr << 0
          << -1 << 0 << arma::endr;
      const arma::mat33 C2(tmpMat);
      // For step 8:
      tmpMat << 1 << 0 << 1 << arma::endr << 0 << 1 << 1 << arma::endr << 0 << 0
          << 1 << arma::endr;
      const arma::mat33 C8(tmpMat);

      // initial change of basis matrix
      tmpMat << 1 << 0 << 0 << arma::endr << 0 << 1 << 0 << arma::endr << 0 << 0
          << 1 << arma::endr;
      arma::mat33 cob(tmpMat);

#define NIGGLI_DEBUG(step)
      unsigned int iter;
      for(iter = 0; iter < iterations; ++iter)
      {
        // Step 1:
        if(stable::gt(A, B, tol)
            || (stable::eq(A, B, tol) && stable::gt(fabs(xi), fabs(eta), tol)))
        {
          cob *= C1;
          std::swap(A, B);
          std::swap(xi, eta);
          NIGGLI_DEBUG(1);
        }

        // Step 2:
        if(stable::gt(B, C, tol)
            || (stable::eq(B, C, tol) && stable::gt(fabs(eta), fabs(zeta), tol)))
        {
          cob *= C2;
          std::swap(B, C);
          std::swap(eta, zeta);
          NIGGLI_DEBUG(2);
          continue;
        }

        // Step 3:
        // Use exact comparisons in steps 3 and 4.
        if(xi * eta * zeta > 0)
        {
          // Update change of basis matrix:
          tmpMat << stable::sign(xi) << 0 << 0 << arma::endr << 0
              << stable::sign(eta) << 0 << arma::endr << 0 << 0
              << stable::sign(zeta) << arma::endr;
          cob *= tmpMat;

          // Update characteristic
          xi = fabs(xi);
          eta = fabs(eta);
          zeta = fabs(zeta);
          NIGGLI_DEBUG(3);
          ++iter;
        }

        // Step 4:
        // Use exact comparisons for steps 3 and 4
        else
        { // either step 3 or 4 should run
          // Update change of basis matrix:
          double *p = NULL;
          double i = 1;
          double j = 1;
          double k = 1;
          if(xi > 0)
          {
            i = -1;
          }
          else if(!(xi < 0))
            p = &i;
          if(eta > 0)
            j = -1;
          else if(!(eta < 0))
            p = &j;
          if(zeta > 0)
            k = -1;
          else if(!(zeta < 0))
            p = &k;
          if(i * j * k < 0)
          {
            if(!p)
            {
              //QMessageBox::warning
              //    (m_mainwindow,
              //     CE_DIALOG_TITLE,
              //     tr("Niggli-reduction failed. The input structure's "
              //        "lattice that is confusing the Niggli-reduction "
              //        "algorithm. Try making a small perturbation (approx."
              //        " 2 orders of magnitude smaller than the tolerance) "
              //        "to the input lattices and try again."));
              return false;
            }
            *p = -1;
          }
          tmpMat << i << 0 << 0 << arma::endr << 0 << j << 0 << arma::endr << 0
              << 0 << k << arma::endr;
          cob *= tmpMat;

          // Update characteristic
          xi = -fabs(xi);
          eta = -fabs(eta);
          zeta = -fabs(zeta);
          NIGGLI_DEBUG(4);
          ++iter;
        }

        // Step 5:
        if(stable::gt(fabs(xi), B, tol)
            || (stable::eq(xi, B, tol) && stable::lt(2 * eta, zeta, tol))
            || (stable::eq(xi, -B, tol) && stable::lt(zeta, 0, tol)))
        {
          double signXi = stable::sign(xi);
          // Update change of basis matrix:
          tmpMat << 1 << 0 << 0 << arma::endr << 0 << 1 << -signXi << arma::endr
              << 0 << 0 << 1 << arma::endr;
          cob *= tmpMat;

          // Update characteristic
          C = B + C - xi * signXi;
          eta = eta - zeta * signXi;
          xi = xi - 2 * B * signXi;
          NIGGLI_DEBUG(5);
          continue;
        }

        // Step 6:
        if(stable::gt(fabs(eta), A, tol)
            || (stable::eq(eta, A, tol) && stable::lt(2 * xi, zeta, tol))
            || (stable::eq(eta, -A, tol) && stable::lt(zeta, 0, tol)))
        {
          double signEta = stable::sign(eta);
          // Update change of basis matrix:
          tmpMat << 1 << 0 << -signEta << arma::endr << 0 << 1 << 0
              << arma::endr << 0 << 0 << 1 << arma::endr;
          cob *= tmpMat;

          // Update characteristic
          C = A + C - eta * signEta;
          xi = xi - zeta * signEta;
          eta = eta - 2 * A * signEta;
          NIGGLI_DEBUG(6);
          continue;
        }

        // Step 7:
        if(stable::gt(fabs(zeta), A, tol)
            || (stable::eq(zeta, A, tol) && stable::lt(2 * xi, eta, tol))
            || (stable::eq(zeta, -A, tol) && stable::lt(eta, 0, tol)))
        {
          double signZeta = stable::sign(zeta);
          // Update change of basis matrix:
          tmpMat << 1 << -signZeta << 0 << arma::endr << 0 << 1 << 0
              << arma::endr << 0 << 0 << 1 << arma::endr;
          cob *= tmpMat;

          // Update characteristic
          B = A + B - zeta * signZeta;
          xi = xi - eta * signZeta;
          zeta = zeta - 2 * A * signZeta;
          NIGGLI_DEBUG(7);
          continue;
        }

        // Step 8:
        double sumAllButC = A + B + xi + eta + zeta;
        if(stable::lt(sumAllButC, 0, tol)
            || (stable::eq(sumAllButC, 0, tol)
                && stable::gt(2 * (A + eta) + zeta, 0, tol)))
        {
          // Update change of basis matrix:
          cob *= C8;

          // Update characteristic
          C = sumAllButC + C;
          xi = 2 * B + xi + zeta;
          eta = 2 * A + eta + zeta;
          NIGGLI_DEBUG(8);
          continue;
        }

        // Done!
        ret = true;
        break;
      }

      // No change
      if(iter == 0)
      {
        //QMessageBox::information
        //  (m_mainwindow,
        //   CE_DIALOG_TITLE,
        //   tr("This unit cell is already reduced to "
        //      "its canonical Niggli representation."));
        return false;
      }

      if(!ret)
      {
        //QMessageBox::warning
        //  (m_mainwindow,
        //   CE_DIALOG_TITLE,
        //   tr("Failed to reduce cell after 1000 iterations of "
        //      "the reduction algorithm. Stopping."));
        return false;
      }

      SSLIB_ASSERT_MSG(arma::det(cob) == 1,
          "Determinant of change of basis matrix must be 1.");

      // Update cell
      init(myOrthoMtx * cob);
      //setCurrentCellMatrix(cob.transpose() * currentCellMatrix());

      // TODO: The check below fails with large unit cells because the angular error between lattice vectors
      // will be fixed but at larger volumes a small error result in a large error in volume.
      // Need to think up a better scheme.
//      SSLIB_ASSERT_MSG(stable::eq(origVolume / getVolume(), 1.0, 1e-2),
//          "Cell volume changed during Niggli reduction.");

      // fix coordinates
      // Apply COB matrix:
      /* Eigen::Matrix3d invCob;
       cob.computeInverse(&invCob);
       for (QList<Eigen::Vector3d>::iterator it = fcoords.begin(), it_end = fcoords.end();
         it != it_end; ++it)
       {
         *it = invCob * (*it);
       }
       setCurrentFractionalCoords(currentAtomicSymbols(), fcoords);*/

      // wrap:
      //wrapAtomsToCell();
      //orientStandard();
      sendUnitCellChangedMsg();

      return true;
    }

    void
    UnitCell::addListener(UnitCellListener & listener)
    {
      myListeners.insert(&listener);
    }

    bool
    UnitCell::removeListener(UnitCellListener & listener)
    {
      Listeners::const_iterator it = myListeners.find(&listener);
      if(it == myListeners.end())
        return false;

      myListeners.erase(it);
      return true;
    }

    void
    UnitCell::sendUnitCellChangedMsg()
    {
      BOOST_FOREACH(UnitCellListener * const l, myListeners)
        l->onUnitCellChanged(*this);
    }

    void
    UnitCell::sendUnitCellVolChangedMsg(const double oldVol)
    {
      BOOST_FOREACH(UnitCellListener * const l, myListeners)
        l->onUnitCellVolumeChanged(*this, oldVol, getVolume());
    }

    bool
    UnitCell::init(const double a, const double b, const double c,
        const double alpha, const double beta, const double gamma)
    {
      using namespace utility::cell_params_enum;
      using std::abs;

      // Sanity checks on parameters
      SSLIB_ASSERT_MSG(alpha + beta + gamma <= 360.0,
          "Non-physical lattice parameters supplied - required that alpha+beta+gamma <= 360.0");
      SSLIB_ASSERT_MSG(abs(alpha - beta) <= gamma,
          "Non-physical lattice parameters supplied - require that abs(alpha-beta) < gamma");
      SSLIB_ASSERT_MSG(abs(beta - gamma) <= alpha,
          "Non-physical lattice parameters supplied - require that abs(beta-gamma) < alpha");
      SSLIB_ASSERT_MSG(abs(gamma - alpha) <= beta,
          "Non-physical lattice parameters supplied - require that abs(gamma-alpha) < beta");

      double oldLatticeParams[6];
      memcpy(oldLatticeParams, myLatticeParams, sizeof(double) * 6);

      myLatticeParams[A] = a;
      myLatticeParams[B] = b;
      myLatticeParams[C] = c;
      myLatticeParams[ALPHA] = alpha;
      myLatticeParams[BETA] = beta;
      myLatticeParams[GAMMA] = gamma;

      if(!initOrthoAndFracMatrices())
      {
        // roll-back changes
        memcpy(myLatticeParams, oldLatticeParams, sizeof(double) * 6);
        return false;
      }
      initRest();
      return true;
    }

    bool
    UnitCell::init(const arma::mat33 & orthoMtx)
    {
      try
      {
        myFracMtx = arma::inv(orthoMtx); // throws if orthoMtx is singular
        myOrthoMtx = orthoMtx;
        initLatticeParams();
        initRest();
      }
      catch(const std::runtime_error & /*e*/)
      {
        return false;
      }
      return true;
    }

    bool
    UnitCell::initOrthoAndFracMatrices()
    {
      using namespace utility::cell_params_enum;
      using namespace utility::cart_coords_enum;

      const double alphaRad = constants::DEG_TO_RAD * myLatticeParams[ALPHA];
      const double betaRad = constants::DEG_TO_RAD * myLatticeParams[BETA];
      const double gammaRad = constants::DEG_TO_RAD * myLatticeParams[GAMMA];

      const arma::mat33 oldOrthoMtx = myOrthoMtx;

      myOrthoMtx.zeros();
      // A - col 0
      myOrthoMtx.at(X, A) = myLatticeParams[A];
      // B - col 1
      myOrthoMtx.at(X, B) = myLatticeParams[B] * cos(gammaRad);
      myOrthoMtx.at(Y, B) = myLatticeParams[B] * sin(gammaRad);
      // C - col 2
      myOrthoMtx.at(X, C) = myLatticeParams[C] * cos(betaRad);
      myOrthoMtx.at(Y, C) = myLatticeParams[C]
          * (cos(alphaRad) - cos(betaRad) * cos(gammaRad)) / sin(gammaRad);
      myOrthoMtx.at(Z, C) = sqrt(
          myLatticeParams[C] * myLatticeParams[C]
              - myOrthoMtx(X, C) * myOrthoMtx(X, C)
              - myOrthoMtx(Y, C) * myOrthoMtx(Y, C));

      try
      {
        myFracMtx = inv(myOrthoMtx);
      }
      catch(const std::runtime_error & /*e*/)
      {
        myOrthoMtx = oldOrthoMtx; // rollback changes
        return false;
      }
      return true;
    }

    void
    UnitCell::initLatticeParams()
    {
      using namespace utility::cell_params_enum;
      using namespace arma;

      // Get the lattice vectors
      const arma::vec3 a = myOrthoMtx.col(A);
      const arma::vec3 b = myOrthoMtx.col(B);
      const arma::vec3 c = myOrthoMtx.col(C);

      myLatticeParams[A] = std::sqrt(dot(a, a));
      myLatticeParams[B] = std::sqrt(dot(b, b));
      myLatticeParams[C] = std::sqrt(dot(c, c));
      myLatticeParams[ALPHA] = acos(
          dot(b, c) / (myLatticeParams[B] * myLatticeParams[C]))
          * constants::RAD_TO_DEG;
      myLatticeParams[BETA] = acos(
          dot(a, c) / (myLatticeParams[A] * myLatticeParams[C]))
          * constants::RAD_TO_DEG;
      myLatticeParams[GAMMA] = acos(
          dot(a, b) / (myLatticeParams[A] * myLatticeParams[B]))
          * constants::RAD_TO_DEG;
    }

    void
    UnitCell::initRest()
    {
      using namespace utility::cell_params_enum;

      myVolume = std::fabs(
          dot(myOrthoMtx.col(A), cross(myOrthoMtx.col(B), myOrthoMtx.col(C))));
    }

    }
    }
