/*
 * UniversalCrystalDistanceCalculator.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "spl/common/UniversalCrystalDistanceCalculator.h"

namespace spl {
namespace common {

UniversalCrystalDistanceCalculator::UniversalCrystalDistanceCalculator(
    UnitCell * const unitCell):
        myUnitCell(NULL)
{
  SSLIB_ASSERT(unitCell);

  setUnitCell(unitCell);
}

UniversalCrystalDistanceCalculator::~UniversalCrystalDistanceCalculator()
{
  if(myUnitCell)
    myUnitCell->removeListener(*this);
}

::arma::vec3
UniversalCrystalDistanceCalculator::getVecMinImg(const arma::vec3 & a,
    const arma::vec3 & b, const unsigned int maxCellMultiples) const
{
  const UnitCell & cell = *myUnitCell;

  // Make sure cart1 and 2 are in the unit cell at the origin
  const arma::vec3 dR = cell.wrapVec(b) - cell.wrapVec(a);
  double minModDRSq = dot(dR, dR);
  const double minModDR = std::sqrt(minModDRSq);

  // Maximum multiple of cell vectors we need to go to
  int A_max = static_cast< int>(std::ceil(
      getNumPlaneRepetitionsToBoundSphere(myCache.B, myCache.C, minModDR)));
  int B_max = static_cast< int>(std::ceil(
      getNumPlaneRepetitionsToBoundSphere(myCache.A, myCache.C, minModDR)));
  int C_max = static_cast< int>(std::ceil(
      getNumPlaneRepetitionsToBoundSphere(myCache.A, myCache.B, minModDR)));

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_max, maxCellMultiples);

  // Loop variables
  arma::vec3 minDR = dR;
  double modDRSq;
  arma::vec3 nA, nAPlusNB, dRImg;
  for(int a = -A_max; a <= A_max; ++a)
  {
    nA = a * myCache.A;
    for(int b = -B_max; b <= B_max; ++b)
    {
      nAPlusNB = nA + b * myCache.B;
      for(int c = -C_max; c <= C_max; ++c)
      {
        dRImg = nAPlusNB + c * myCache.C + dR;

        modDRSq = dot(dRImg, dRImg);
        if(modDRSq < minModDRSq)
        {
          minModDRSq = modDRSq;
          minDR = dRImg;
        }
      }
    }
  }

  return minDR;
}

bool
UniversalCrystalDistanceCalculator::getDistsBetween(const arma::vec3 & a,
    const arma::vec3 & b, const double cutoff,
    std::vector< double> & outValues, const size_t maxValues,
    const unsigned int maxCellMultiples) const
{
  const UnitCell & cell = *myUnitCell;
  const double vol = cell.getVolume();
  const arma::vec3 dR = b - a;

  int A_max = static_cast< int>(std::floor(
      getNumPlaneRepetitionsToBoundSphere(
          cutoff + std::abs(arma::dot(dR, myCache.bCrossCHat)), vol,
          myCache.bCrossCLen)));
  int B_max = static_cast< int>(std::floor(
      getNumPlaneRepetitionsToBoundSphere(
          cutoff + std::abs(arma::dot(dR, myCache.aCrossCHat)), vol,
          myCache.aCrossCLen)));
  int C_max = static_cast< int>(std::floor(
      getNumPlaneRepetitionsToBoundSphere(
          cutoff + std::abs(arma::dot(dR, myCache.aCrossBHat)), vol,
          myCache.aCrossBLen)));

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_max, maxCellMultiples);

  const double cutoffSq = cutoff * cutoff;
  size_t numFound = 0;
  arma::vec3 rA, rAB, outVec;
  double testDistSq;
  for(int a = -A_max; a <= A_max; ++a)
  {
    rA = a * myCache.A;
    for(int b = -B_max; b <= B_max; ++b)
    {
      rAB = rA + b * myCache.B;
      for(int c = -C_max; c <= C_max; ++c)
      {
        outVec = rAB + c * myCache.C + dR;
        testDistSq = arma::dot(outVec, outVec);

        if(testDistSq < cutoffSq)
        {
          outValues.push_back(std::sqrt(testDistSq));
          if(++numFound >= maxValues)
            return false;
        }
      }
    }
  }

  // Completed successfully
  return !problemDuringCalculation;
}

bool
UniversalCrystalDistanceCalculator::getVecsBetween(const arma::vec3 & a,
    const arma::vec3 & b, const double cutoff,
    std::vector< arma::vec3> & outValues, const size_t maxValues,
    const unsigned int maxCellMultiples) const
{
  const UnitCell & cell = *myUnitCell;
  const double vol = cell.getVolume();
  const arma::vec3 dR = b - a;

  int A_max = static_cast< int>(std::floor(
      getNumPlaneRepetitionsToBoundSphere(
          cutoff + std::abs(arma::dot(dR, myCache.bCrossCHat)), vol,
          myCache.bCrossCLen)));
  int B_max = static_cast< int>(std::floor(
      getNumPlaneRepetitionsToBoundSphere(
          cutoff + std::abs(arma::dot(dR, myCache.aCrossCHat)), vol,
          myCache.aCrossCLen)));
  int C_max = static_cast< int>(std::floor(
      getNumPlaneRepetitionsToBoundSphere(
          cutoff + std::abs(arma::dot(dR, myCache.aCrossBHat)), vol,
          myCache.aCrossBLen)));

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_max, maxCellMultiples);

  const double cutoffSq = cutoff * cutoff;
  arma::vec3 outVec;
  size_t numFound = 0;
  arma::vec3 rA, rAB;
  for(int a = -A_max; a <= A_max; ++a)
  {
    rA = a * myCache.A;
    for(int b = -B_max; b <= B_max; ++b)
    {
      rAB = rA + b * myCache.B;
      for(int c = -C_max; c <= C_max; ++c)
      {
        outVec = rAB + c * myCache.C + dR;

        if(arma::dot(outVec, outVec) < cutoffSq)
        {
          outValues.push_back(outVec);
          if(++numFound >= maxValues)
            return false;
        }
      }
    }
  }

  // Completed successfully
  return !problemDuringCalculation;
}

bool
UniversalCrystalDistanceCalculator::isValid() const
{
  return myUnitCell;
}

void
UniversalCrystalDistanceCalculator::setUnitCell(common::UnitCell * const unitCell)
{
  if(myUnitCell == unitCell)
    return;

  if(myUnitCell)
    myUnitCell->removeListener(*this);

  myUnitCell = unitCell;
  if(myUnitCell)
  {
    myCache.update(*myUnitCell);
    myUnitCell->addListener(*this);
  }
}

double
UniversalCrystalDistanceCalculator::getNumPlaneRepetitionsToBoundSphere(
    const arma::vec3 & planeVec1, const arma::vec3 & planeVec2,
    const double radius) const
{
  // The vector normal to the plane
  const arma::vec3 normal = arma::cross(planeVec1, planeVec2);
  const double unitCellVolume = myUnitCell->getVolume(); // = a . |b x c|

  return radius / unitCellVolume * std::sqrt(arma::dot(normal, normal));
}

void
UniversalCrystalDistanceCalculator::onUnitCellChanged(UnitCell & unitCell)
{
  SSLIB_ASSERT(myUnitCell == &unitCell);

  myCache.update(*myUnitCell);
}

void
UniversalCrystalDistanceCalculator::onUnitCellVolumeChanged(UnitCell & unitCell,
    const double oldVol, const double newVol)
{
  SSLIB_ASSERT(myUnitCell == &unitCell);

  myCache.update(*myUnitCell);
}

void
UniversalCrystalDistanceCalculator::onUnitCellDestroyed()
{
  myUnitCell = NULL;
}

void
UniversalCrystalDistanceCalculator::Cache::update(const UnitCell & cell)
{
  A = cell.getAVec();
  B = cell.getBVec();
  C = cell.getCVec();

  aCrossB = arma::cross(A, B);
  aCrossBLen = std::sqrt(arma::dot(aCrossB, aCrossB));
  aCrossBHat = aCrossB / aCrossBLen;

  bCrossC = arma::cross(B, C);
  bCrossCLen = std::sqrt(arma::dot(bCrossC, bCrossC));
  bCrossCHat = bCrossC / bCrossCLen;

  aCrossC = arma::cross(A, C);
  aCrossCLen = std::sqrt(arma::dot(aCrossC, aCrossC));
  aCrossCHat = aCrossC / aCrossCLen;
}

}
}
