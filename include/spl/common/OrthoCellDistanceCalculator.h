/*
 * OrthoCellDistanceCalculator.h
 *
 *  Created on: Sep 6, 2011
 *      Author: Martin Uhrin
 */

#ifndef ORTHO_CELL_DISTANCE_CALCULATOR_H
#define ORTHO_CELL_DISTANCE_CALCULATOR_H

// INCLUDES ///////////////////////////////////
#include "spl/SSLib.h"

#include <boost/noncopyable.hpp>

#include <armadillo>

#include "spl/SSLibAssert.h"
#include "spl/common/DistanceCalculator.h"
#include "spl/common/UnitCell.h"

namespace spl {
namespace common {

class OrthoCellDistanceCalculator : public DistanceCalculator,
    UnitCell::UnitCellListener
{
public:
  static const double VALID_ANGLE_TOLERANCE;

  explicit
  OrthoCellDistanceCalculator(UnitCell * const unitCell);
  virtual
  ~OrthoCellDistanceCalculator();

  using DistanceCalculator::getDistsBetween;
  using DistanceCalculator::getVecsBetween;

  virtual bool
  getDistsBetween(const ::arma::vec3 & r1, const ::arma::vec3 & r2,
      const double cutoff, ::std::vector< double> & outDistances,
      const size_t maxDistances = DEFAULT_MAX_OUTPUTS,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual ::arma::vec3
  getVecMinImg(const ::arma::vec3 & r1, const ::arma::vec3 & r2,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool
  getVecsBetween(const ::arma::vec3 & r1, const ::arma::vec3 & r2,
      const double cutoff, ::std::vector< ::arma::vec3> & outVectors,
      const size_t maxVectors = DEFAULT_MAX_OUTPUTS,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool
  isValid() const;

  void
  setUnitCell(common::UnitCell * const unitCell);

  virtual void
  onUnitCellChanged(UnitCell & unitCell);
  virtual void
  onUnitCellVolumeChanged(UnitCell & unitCell, const double oldVol,
      const double newVol);
  virtual void
  onUnitCellDestroyed();

private:
  void
  updateBufferedValues();

  ::arma::vec3 myA;
  ::arma::vec3 myB;
  ::arma::vec3 myC;

  ::arma::vec3 myANorm;
  ::arma::vec3 myBNorm;
  ::arma::vec3 myCNorm;

  double myARecip;
  double myBRecip;
  double myCRecip;

  common::UnitCell * myUnitCell;
};

} // namespace common
} // namespace spl

#endif /* ORTHO_CELL_DISTANCE_CALCULATOR_H */
