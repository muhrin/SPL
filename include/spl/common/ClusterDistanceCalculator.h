/*
 * ClusterDistanceCalculator.h
 *
 *  Created on: Sep 4, 2012
 *      Author: Martin Uhrin
 */

#ifndef CLUSTER_DISTANCE_CALCULATOR_H
#define CLUSTER_DISTANCE_CALCULATOR_H

// INCLUDES ///////////////////////////////////
#include "spl/common/DistanceCalculator.h"

#include "spl/common/Structure.h"

namespace spl {
namespace common {

class ClusterDistanceCalculator : public DistanceCalculator
{
public:
  virtual inline arma::vec3
  getVecMinImg(const arma::vec3 & a, const arma::vec3 & b,
      const unsigned int /* maxCellMultiples */= DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return b - a;
  }

  virtual bool
  getDistsBetween(const arma::vec3 & a, const arma::vec3 & b,
      const double cutoff, std::vector< double> & outDistances,
      const size_t maxDistances = DEFAULT_MAX_OUTPUTS,
      const unsigned int /* maxCellMultiples */= DEFAULT_MAX_CELL_MULTIPLES) const
  {
    const double cutoffSq = cutoff * cutoff;
    const arma::vec dr = b - a;
    const double distSq = arma::dot(dr, dr);

    if(distSq < cutoffSq)
      outDistances.push_back(std::sqrt(distSq));

    return true;
  }

  virtual inline bool
  getVecsBetween(const arma::vec3 & a, const arma::vec3 & b,
      const double cutoff, std::vector< arma::vec3> & outVectors,
      const size_t maxVectors = DEFAULT_MAX_OUTPUTS,
      const unsigned int /* maxCellMultiples */= DEFAULT_MAX_CELL_MULTIPLES) const
  {
    const double cutoffSq = cutoff * cutoff;
    const arma::vec dr = b - a;

    if(arma::dot(dr, dr) < cutoffSq)
      outVectors.push_back(dr);

    return true;
  }

  virtual inline bool
  isValid() const
  {
    return true;
  }
};

}
} // Close the namespace

#endif /* CLUSTER_DISTANCE_CALCULATOR_H */
