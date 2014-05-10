/*
 * UniversalCrystalDistanceCalculator.h
 *
 *  Created on: Sep 4, 2012
 *      Author: Martin Uhrin
 */

#ifndef UNIVERSAL_CRYSTAL_DISTANCE_CALCULATOR_H
#define UNIVERSAL_CRYSTAL_DISTANCE_CALCULATOR_H

// INCLUDES ///////////////////////////////////
#include <boost/noncopyable.hpp>

#include "spl/common/DistanceCalculator.h"
#include "spl/common/UnitCell.h"

namespace spl {
namespace common {

class UniversalCrystalDistanceCalculator : public DistanceCalculator,
    UnitCell::UnitCellListener
{
public:

  explicit
  UniversalCrystalDistanceCalculator(UnitCell * const unitCell);
  virtual
  ~UniversalCrystalDistanceCalculator();

  using DistanceCalculator::getDistsBetween;
  using DistanceCalculator::getVecsBetween;

  virtual arma::vec3
  getVecMinImg(const arma::vec3 & a, const arma::vec3 & b,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool
  getDistsBetween(const arma::vec3 & a, const arma::vec3 & b,
      const double cutoff, std::vector< double> & outDistances,
      const size_t maxDistances = DistanceCalculator::DEFAULT_MAX_OUTPUTS,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool
  getVecsBetween(const arma::vec3 & a, const arma::vec3 & b,
      const double cutoff, std::vector< arma::vec3> & outVectors,
      const size_t maxVectors = DistanceCalculator::DEFAULT_MAX_OUTPUTS,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool
  isValid() const;

  virtual void
  setUnitCell(common::UnitCell * const unitCell);

private:
  struct Cache
  {
    void
    update(const UnitCell & cell);

    arma::vec3 A;
    arma::vec3 B;
    arma::vec3 C;

    arma::vec3 aCrossB;
    arma::vec3 bCrossC;
    arma::vec3 aCrossC;

    double aCrossBLen;
    double bCrossCLen;
    double aCrossCLen;

    arma::vec3 aCrossBHat;
    arma::vec3 bCrossCHat;
    arma::vec3 aCrossCHat;
  };

  double
  getNumPlaneRepetitionsToBoundSphere(const arma::vec3 & planeVec1,
      const arma::vec3 & planeVec2, const double radius) const;
  inline double
  getNumPlaneRepetitionsToBoundSphere(const double radius, const double volume,
      const double crossLen) const
  {
    return radius / volume * crossLen;
  }

  virtual void
  onUnitCellChanged(UnitCell & unitCell);
  virtual void
  onUnitCellVolumeChanged(UnitCell & unitCell, const double oldVol,
      const double newVol);
  virtual void
  onUnitCellDestroyed();

  common::UnitCell * myUnitCell;
  Cache myCache;
};

} // namespace common
} // namespace spl

#endif /* UNIVERSAL_CRYSTAL_DISTANCE_CALCULATOR_H */
