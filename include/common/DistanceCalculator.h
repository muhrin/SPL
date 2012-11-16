/*
 * DistanceCalculator.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef DISTANCE_CALCULATOR_H
#define DISTANCE_CALCULATOR_H

// INCLUDES ///////////////////////////////////
#include "SSLib.h"

#include <boost/noncopyable.hpp>

#include "SSLibAssert.h"
#include "common/Atom.h"
#include "common/Types.h"

namespace sstbx {
namespace common {

class Structure;

class DistanceCalculator : private ::boost::noncopyable
{
public:

  static const size_t DEFAULT_MAX_OUTPUTS;
  static const unsigned int DEFAULT_MAX_CELL_MULTIPLES;

  explicit DistanceCalculator(const Structure & structure);
  virtual ~DistanceCalculator() {}

  virtual inline double getDistMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  { return sqrt(getDistSqMinImg(a, b, maxCellMultiples)); }

  virtual inline double getDistMinImg(const Atom & atom1, const Atom & atom2, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  { return getDistMinImg(atom1.getPosition(), atom2.getPosition(), maxCellMultiples); }

  virtual inline double getDistSqMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    const ::arma::vec3 dr = getVecMinImg(a, b, maxCellMultiples);
    return ::arma::dot(dr, dr);
  }

  virtual inline double getDistSqMinImg(const Atom & atom1, const Atom & atom2, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  { return getDistSqMinImg(atom1.getPosition(), atom2.getPosition(), maxCellMultiples); }

  virtual bool getDistsBetween(
    const ::arma::vec3 & a,
    const ::arma::vec3 & b,
    const double cutoff,
    ::std::vector<double> & outDistances,
    const size_t maxDistances = DEFAULT_MAX_OUTPUTS,
    const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const = 0;

  virtual inline bool getDistsBetween(
    const Atom & atom1,
    const Atom & atom2,
    const double cutoff,
    ::std::vector<double> & outDistances,
    const size_t maxDistances = DEFAULT_MAX_OUTPUTS,
    const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  {
    return getDistsBetween(atom1.getPosition(), atom2.getPosition(), cutoff, outDistances, maxDistances, maxCellMultiples);
  }

  virtual ::arma::vec3 getVecMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const = 0;

  virtual ::arma::vec3 getVecMinImg(const Atom & atom1, const Atom & atom2, const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  { return getVecMinImg(atom1.getPosition(), atom2.getPosition(), maxCellMultiples); }

  virtual bool getVecsBetween(
    const ::arma::vec3 & a,
    const ::arma::vec3 & b,
    const double cutoff,
    ::std::vector< ::arma::vec3> & outVectors,
    const size_t maxVectors = DEFAULT_MAX_OUTPUTS,
    const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const = 0;

  virtual inline bool getVecsBetween(
    const Atom & atom1,
    const Atom & atom2,
    const double cutoff,
    ::std::vector< ::arma::vec3> & outVectors,
    const size_t maxVectors = DEFAULT_MAX_OUTPUTS,
    const unsigned int maxCellMultiples = DEFAULT_MAX_CELL_MULTIPLES) const
  { return getVecsBetween(atom1.getPosition(), atom2.getPosition(), cutoff, outVectors, maxVectors, maxCellMultiples); }

  virtual bool isValid() const = 0;

  virtual void unitCellChanged() {};

protected:

  inline bool capMultiples(int & max, const unsigned int cap) const
  {
    const int iCap = (int)cap;
    if(max > iCap)
    {
      max = iCap;
      return true;
    }
    return false;
  }

  inline bool capMultiples(int & min, int & max, const unsigned int cap) const
  {
    SSLIB_ASSERT(min <= max);
    const int iCap = (int)cap;

    bool capped = false;
    if(min < -iCap)
    {
      min = -iCap;
      capped = true;
    }
    if(max > iCap)
    {
      max = iCap;
      capped = true;
    }
    return capped;
  }

  const Structure & myStructure;

};

} // namespace common
} // namespace sstbx

#endif /* DISTANCE_CALCULATOR_H */
