/*
 * DistanceCalculatorDelegator.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef DISTANCE_CALCULATOR_DELEGATOR_H
#define DISTANCE_CALCULATOR_DELEGATOR_H

// INCLUDES ///////////////////////////////////
#include "spl/SSLib.h"

#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>

#include <armadillo>

#include "spl/common/DistanceCalculator.h"

namespace spl {
namespace common {

// FORWARD DECLARES ///////////////////////////
class Atom;
class Structure;
class UnitCell;

class DistanceCalculatorDelegator : public DistanceCalculator
{
public:
  DistanceCalculatorDelegator(Structure & structure);

  // From DistanceCalculator /////////////////////

  virtual inline double
  getDistMinImg(const arma::vec3 & a, const arma::vec3 & b,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getDistMinImg(a, b, maxCellMultiples);
  }

  virtual inline double
  getDistMinImg(const Atom & atom1, const Atom & atom2,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getDistMinImg(atom1, atom2, maxCellMultiples);
  }

  virtual inline double
  getDistSqMinImg(const arma::vec3 & a, const arma::vec3 & b,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getDistSqMinImg(a, b, maxCellMultiples);
  }

  virtual inline double
  getDistSqMinImg(const Atom & atom1, const Atom & atom2,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getDistSqMinImg(atom1, atom2, maxCellMultiples);
  }

  virtual inline bool
  getDistsBetween(const arma::vec3 & a, const arma::vec3 & b,
      const double cutoff, std::vector< double> & outDistances,
      const size_t maxDistances = DEFAULT_MAX_OUTPUTS,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getDistsBetween(a, b, cutoff, outDistances, maxDistances,
        maxCellMultiples);
  }

  virtual inline bool
  getDistsBetween(const Atom & atom1, const Atom & atom2, const double cutoff,
      std::vector< double> & outDistances, const size_t maxDistances =
          DEFAULT_MAX_OUTPUTS, const unsigned int maxCellMultiples =
          DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getDistsBetween(atom1, atom2, cutoff, outDistances,
        maxDistances, maxCellMultiples);
  }

  virtual arma::vec3
  getVecMinImg(const arma::vec3 & a, const arma::vec3 & b,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getVecMinImg(a, b, maxCellMultiples);
  }

  virtual arma::vec3
  getVecMinImg(const Atom & atom1, const Atom & atom2,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getVecMinImg(atom1, atom2, maxCellMultiples);
  }

  virtual bool
  getVecsBetween(const arma::vec3 & a, const arma::vec3 & b,
      const double cutoff, std::vector< arma::vec3> & outVectors,
      const size_t maxVectors = DEFAULT_MAX_OUTPUTS,
      const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getVecsBetween(a, b, cutoff, outVectors, maxVectors,
        maxCellMultiples);
  }

  virtual bool
  getVecsBetween(const Atom & atom1, const Atom & atom2, const double cutoff,
      std::vector< arma::vec3> & outVectors, const size_t maxVectors =
          DEFAULT_MAX_OUTPUTS, const unsigned int maxCellMultiples =
          DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return myDelegate->getVecsBetween(atom1, atom2, cutoff, outVectors,
        maxVectors, maxCellMultiples);
  }

  bool
  isValid() const
  {
    return myDelegate->isValid();
  }

  virtual void
  setUnitCell(common::UnitCell * const unitCell);

  // End from DistanceCalculator /////////////////

private:
  struct CalculatorType
  {
    enum Value
    {
      NONE, CLUSTER, UNIVERSAL_CRYSTAL, ORTHO_CELL
    };
  };

  void
  updateDelegate();

  bool
  setDelegate(const CalculatorType::Value calcType);

  Structure & myStructure;
  spl::UniquePtr<DistanceCalculator>::Type myDelegate;
  CalculatorType::Value myDelegateType;
};

}
} // Close the namespace

#endif /* DISTANCE_CALCULATOR_DELEGATOR_H */
