/*
 * ReferenceDistanceCalculator.h
 *
 *  Created on: Sep 4, 2012
 *      Author: Martin Uhrin
 */

#ifndef REFERENCE_DISTANCE_CALCULATOR_H
#define REFERENCE_DISTANCE_CALCULATOR_H

// INCLUDES ///////////////////////////////////
#include "common/DistanceCalculator.h"

namespace sstbx {
namespace common {

class ReferenceDistanceCalculator : public DistanceCalculator
{
public:

  explicit ReferenceDistanceCalculator(const Structure & structure);

  using DistanceCalculator::getDistsBetween;
  using DistanceCalculator::getVecsBetween;

  virtual ::arma::vec3 getVecMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool getDistsBetween(
    const ::arma::vec3 & a,
    const ::arma::vec3 & b,
    const double cutoff,
    ::std::vector<double> & outDistances,
    const size_t maxDistances = DistanceCalculator::DEFAULT_MAX_OUTPUTS,
    const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool getVecsBetween(
    const ::arma::vec3 & a,
    const ::arma::vec3 & b,
    const double cutoff,
    ::std::vector< ::arma::vec3> & outVectors,
    const size_t maxVectors = DistanceCalculator::DEFAULT_MAX_OUTPUTS,
    const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const;

  virtual bool isValid() const;

  virtual void unitCellChanged() {}

private:

	double getNumPlaneRepetitionsToBoundSphere(
    const ::arma::vec3 & boundDir,
    const ::arma::vec3 & planeVecA,
    const ::arma::vec3 & planeVecB,
		const double radius) const;

};

}
} // Close the namespace

#endif /* REFERENCE_DISTANCE_CALCULATOR_H */
