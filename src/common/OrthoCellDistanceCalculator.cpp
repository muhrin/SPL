/*
 * OrthoCellDistanceCalculator.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////////////////////////
#include "common/OrthoCellDistanceCalculator.h"

#include "common/Structure.h"
#include "common/UnitCell.h"
#include "utility/IndexingEnums.h"
#include "utility/StableComparison.h"

#define SSLIB_ORTHO_DIST_CALC_DEBUG (SSLIB_DEBUG && 0)

#if SSLIB_ORTHO_DIST_CALC_DEBUG
#  include <iostream>
#endif

namespace sstbx {
namespace common {

const double OrthoCellDistanceCalculator::VALID_ANGLE_TOLERANCE = 1e-10;

OrthoCellDistanceCalculator::OrthoCellDistanceCalculator(const sstbx::common::Structure &structure):
DistanceCalculator(structure)
{
  updateBufferedValues();
}

bool OrthoCellDistanceCalculator::getDistsBetween(
  const arma::vec3 & r1,
  const arma::vec3 & r2,
  double cutoff,
  std::vector<double> &outDistances,
  const size_t maxDistances,
  const unsigned int maxCellMultiples) const
{
  // The cutoff has to be positive
  cutoff = abs(cutoff);
  const UnitCell & cell = *myStructure.getUnitCell();

	// Get the lattice vectors
  const ::arma::vec3 A(cell.getAVec());
  const ::arma::vec3 B(cell.getBVec());
  const ::arma::vec3 C(cell.getCVec());

  const ::arma::vec3 r12 = cell.wrapVec(r2) - cell.wrapVec(r1);
  const double (&params)[6] = cell.getLatticeParams();

  const double cutoffSq = cutoff * cutoff;

  const double rDotA =  ::arma::dot(r12, myANorm);
  const double rDotB = ::arma::dot(r12, myBNorm);
  const double rDotC = ::arma::dot(r12, myCNorm);

  // Maximum multiples of cell vectors we need to go to
  int A_min = -(int)floor((cutoff + rDotA) * myARecip);
  int A_max = (int)floor((cutoff - rDotA) * myARecip);
  int B_min = -(int)floor((cutoff + rDotB) * myBRecip);
  int B_max = (int)floor((cutoff - rDotB) * myBRecip);
  int C_min = -(int)floor((cutoff + rDotC) * myCRecip);
  int C_max = (int)floor((cutoff - rDotC) * myCRecip);

  // Check if there are any vectors that will be within the cutoff
  if(A_min > A_max || B_min > B_max || C_min > C_max)
    return true;

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_min, A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_min, B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_min, C_max, maxCellMultiples);

  // Loop variables
  size_t numDistances = 0;
  //double dRDistSq;
  ::arma::vec3 nA, nAPlusNB, dRImg;
  double r_x, r_y, r_z, aSq, bSq, testDistSq;

  //if(doFullDistanceCheck)
  //{
	 // for(int a = A_min; a <= A_max; ++a)
	 // {
  //    nA = a * myA;

  //    aSq = a * params[0] + rDotA;
  //    aSq *= aSq;
  //    B_min = -(int)floor((sqrt(cutoffSq + aSq) + rDotB) * myBRecip);
  //    B_max = (int)floor((sqrt(cutoffSq - aSq) - rDotB) * myBRecip);
  //    for(int b = B_min; b <= B_max; ++b)
	//    {
  //      nAPlusNB = nA + b * myB;

  //      bSq = b * params[1] + rDotB;
  //      bSq *= bSq;
  //      C_min = -(int)floor((sqrt(cutoffSq + aSq + bSq) + rDotC) * myCRecip);
  //      C_max = (int)floor((sqrt(cutoffSq - aSq - bSq) - rDotC) * myCRecip);
  //	    for(int c = C_min; c <= C_max; ++c)
  //	    {
  //        dRImg = c * myC + nAPlusNB + r12;
  //        dRDistSq = ::arma::dot(dRImg, dRImg);

		//		  if(dRDistSq < cutoffSq)
		//		  {
  //          outDistances.push_back(sqrt(dRDistSq));
  //          if(++numDistances >= maxDistances)
  //            return false;
		//		  }
		//	  }
		//  }
	 // }
  //}
  //else
  {
    for(int a = A_min; a <= A_max; ++a)
    {
      r_x = a * params[0] + rDotA;
      aSq = r_x * r_x;
	    for(int b = B_min; b <= B_max; ++b)
      {
        r_y = b * params[1] + rDotB;
        bSq = r_y * r_y;
        if(aSq + bSq < cutoffSq)
        {
		      for(int c = C_min; c <= C_max; ++c)
		      {
            r_z = c * params[2] + rDotC;
            testDistSq = aSq + bSq + r_z * r_z;

#if SSLIB_ORTHO_DIST_CALC_DEBUG
            ::arma::vec3 testVec = a * A + b * B + c * C + r12;
            const double vecLengthSq = ::arma::dot(testVec, testVec);
            if(vecLengthSq != testDistSq)
              ::std::cout << "Error: Distance vectors do not match\n";
#endif

            if(testDistSq < cutoffSq)
			      {
              outDistances.push_back(sqrt(testDistSq));
              if(++numDistances >= maxDistances)
                return false;
			      }
          }
        }
      }
    }
  }

  return !problemDuringCalculation;
}

::arma::vec3 OrthoCellDistanceCalculator::getVecMinImg(const ::arma::vec3 & r1, const ::arma::vec3 & r2, const unsigned int /*maxCellMultiples*/) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

  const ::arma::mat33 & fracMtx = cell.getFracMtx();
  const ::arma::mat33 & orthoMtx = cell.getOrthoMtx();

  ::arma::vec3 r12 = r2 - r1;

  r12 = fracMtx * r12;
  r12[0] -= floor(r12[0] + 0.5);
  r12[1] -= floor(r12[1] + 0.5);
  r12[2] -= floor(r12[2] + 0.5);
  r12 = orthoMtx * r12;

  return r12;
}

bool OrthoCellDistanceCalculator::getVecsBetween(
  const ::arma::vec3 & r1,
  const ::arma::vec3 & r2,
  double cutoff,
  ::std::vector< ::arma::vec3> & outVectors,
  const size_t maxValues,
  const unsigned int maxCellMultiples) const
{
  // The cutoff has to be positive
  cutoff = abs(cutoff);
  const UnitCell & cell = *myStructure.getUnitCell();

  const ::arma::vec3 r12 = cell.wrapVec(r2) - cell.wrapVec(r1);
  const double (&params)[6] = cell.getLatticeParams();

  const double cutoffSq = cutoff * cutoff;

  const double rDotA =  ::arma::dot(r12, myANorm);
  const double rDotB = ::arma::dot(r12, myBNorm);
  const double rDotC = ::arma::dot(r12, myCNorm);

  // Maximum multiples of cell vectors we need to go to
  int A_min = -(int)floor((cutoff + rDotA) * myARecip);
  int A_max = (int)floor((cutoff - rDotA) * myARecip);
  int B_min = -(int)floor((cutoff + rDotB) * myBRecip);
  int B_max = (int)floor((cutoff - rDotB) * myBRecip);
  int C_min = -(int)floor((cutoff + rDotC) * myCRecip);
  int C_max = (int)floor((cutoff - rDotC) * myCRecip);

  // Check if there are any vectors that will be within the cutoff
  if(A_min > A_max || B_min > B_max || C_min > C_max)
    return true;

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_min, A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_min, B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_min, C_max, maxCellMultiples);

  // Loop variables
  size_t numVectors = 0;
  ::arma::vec3 dR, outVec;
  double r_x, r_y, r_z, aSq, bSq, testDistSq;
  for(int a = A_min; a <= A_max; ++a)
  {
    r_x = a * params[0] + rDotA;
    aSq = r_x * r_x;
	  for(int b = B_min; b <= B_max; ++b)
    {
      r_y = b * params[1] + rDotB;
      bSq = r_y * r_y;
      if(aSq + bSq < cutoffSq)
      {
		    for(int c = C_min; c <= C_max; ++c)
		    {
          r_z = c * params[2] + rDotC;
          testDistSq = aSq + bSq + r_z * r_z;

          if(testDistSq < cutoffSq)
			    {
            outVec[0] = r_x;
            outVec[1] = r_y;
            outVec[2] = r_z;
            outVectors.push_back(outVec);
            if(++numVectors >= maxValues)
              return false;
			    }
        }
      }
    }
  }

  return !problemDuringCalculation;

  //double dRDistSq;
	//for(int a = A_min; a <= A_max; ++a)
	//{
 //   nA = a * myA;

 //   aSq = a * params[0] + rDotA;
 //   aSq *= aSq;
 //   B_min = -(int)floor((sqrt(cutoffSq + aSq) + rDotB) / params[1]);
 //   B_max = (int)floor((sqrt(cutoffSq - aSq) - rDotB) / params[1]);
	//	for(int b = B_min; b <= B_max; ++b)
	//	{
 //     nAPlusNB = nA + b * myB;

 //     bSq = b * params[1] + rDotB;
 //     bSq *= bSq;
 //     C_min = -(int)floor((sqrt(cutoffSq + aSq + bSq) + rDotC) / params[2]);
 //     C_max = (int)floor((sqrt(cutoffSq - aSq - bSq) - rDotC) / params[2]);
	//		for(int c = C_min; c <= C_max; ++c)
	//		{
 //       dRImg = c * myC + nAPlusNB + r12;
 //       dRDistSq = ::arma::dot(dRImg, dRImg);

	//			if(dRDistSq < cutoffSq)
	//			{
 //         outVectors.push_back(dRImg);
 //         if(++numVectors >= maxVectors)
 //           return false;
	//			}
	//		}
	//	}
	//}
}

bool OrthoCellDistanceCalculator::isValid() const
{
  using namespace utility::cell_params_enum;

  if(!myStructure.getUnitCell())
    return false;

  const double (&params)[6] = myStructure.getUnitCell()->getLatticeParams();

  // All angles equal 90
  return
    utility::StableComp::eq(params[ALPHA], 90.0, VALID_ANGLE_TOLERANCE) &&
    utility::StableComp::eq(params[BETA], 90.0, VALID_ANGLE_TOLERANCE) &&
    utility::StableComp::eq(params[GAMMA], 90.0, VALID_ANGLE_TOLERANCE);
}

void OrthoCellDistanceCalculator::unitCellChanged()
{
  updateBufferedValues();
}

void OrthoCellDistanceCalculator::updateBufferedValues()
{
  using namespace utility::cell_params_enum;

  const UnitCell & cell = *myStructure.getUnitCell();
  const double (&params)[6] = cell.getLatticeParams();

  myA = cell.getAVec();
  myB = cell.getBVec();
  myC = cell.getCVec();

  myANorm = myA / params[A];
  myBNorm = myB / params[B];
  myCNorm = myC / params[C];

  myARecip = 1.0 / params[A];
  myBRecip = 1.0 / params[B];
  myCRecip = 1.0 / params[C];
}


}
}
