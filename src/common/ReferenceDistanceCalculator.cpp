/*
 * ReferenceDistanceCalculator.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/ReferenceDistanceCalculator.h"

#include "common/Structure.h"
#include "common/UnitCell.h"


namespace sstbx {
namespace common {

ReferenceDistanceCalculator::ReferenceDistanceCalculator(const Structure & structure):
DistanceCalculator(structure)
{}

::arma::vec3 ReferenceDistanceCalculator::getVecMinImg(const ::arma::vec3 & r1, const ::arma::vec3 & r2, const unsigned int maxCellMultiples) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

	// Make sure cart1 and 2 are in the unit cell at the origin
  const ::arma::vec3		dR		= cell.wrapVec(r2) - cell.wrapVec(r1);
	double minModDRSq = dot(dR, dR);
	const double minModDR = sqrt(minModDRSq);

	// Get the lattice vectors
  const ::arma::vec3 A(cell.getAVec());
  const ::arma::vec3 B(cell.getBVec());
  const ::arma::vec3 C(cell.getCVec());

	// Maximum multiple of cell vectors we need to go to
	int A_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(A, B, C, minModDR));
	int B_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(B, A, C, minModDR));
	int C_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(C, A, B, minModDR));

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_max, maxCellMultiples);

  // Loop variables
  ::arma::vec3 minDR = dR;
  const ::arma::mat33 & orthoMtx = cell.getOrthoMtx();
	double modDRSq;
  ::arma::vec3 nA, nAPlusNB, dRImg;
  size_t numDistances = 0;
	for(int a = -A_max; a <= A_max; ++a)
	{
		nA = a * A;
		for(int b = -B_max; b <= B_max; ++b)
		{
		  nAPlusNB = nA + b * B;
			for(int c = -C_max; c <= C_max; ++c)
			{
        dRImg = nAPlusNB + c * C + dR;
				
				modDRSq = dot(dRImg, dRImg);
				if(modDRSq < minModDRSq)
				{
					minModDRSq	= modDRSq;
					minDR		= dRImg;
				}
			}
		}
	}

	return minDR;
}

bool ReferenceDistanceCalculator::getDistsBetween(
    const ::arma::vec3 & r1,
    const ::arma::vec3 & r2,
    const double cutoff,
    ::std::vector<double> & outValues,
    const size_t maxValues,
    const unsigned int maxCellMultiples) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

	// Make sure a and b are in the unit cell at the origin
  const ::arma::vec3		dR		= cell.wrapVec(r2) - cell.wrapVec(r1);

	// Get the lattice vectors
  const ::arma::vec3 A(cell.getAVec());
  const ::arma::vec3 B(cell.getBVec());
  const ::arma::vec3 C(cell.getCVec());

  const double safeCutoff = cutoff + sqrt(::arma::dot(dR, dR));

	// Maximum multiple of cell vectors we need to go to
	int A_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(A, B, C, safeCutoff));
	int B_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(B, A, C, safeCutoff));
	int C_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(C, A, B, safeCutoff));

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_max, maxCellMultiples);

  const double cutoffSq = cutoff * cutoff;
  ::arma::vec3 outVec;
  size_t numFound = 0;
  ::arma::vec3 r;
  double rSq;
	for(int a = -A_max; a <= A_max; ++a)
	{
		for(int b = -B_max; b <= B_max; ++b)
		{
			for(int c = -C_max; c <= C_max; ++c)
			{
        r = a * A + b * B + c * C + dR;
        rSq = ::arma::dot(r, r);

				if(rSq < cutoffSq)
        {
          outValues.push_back(sqrt(rSq));
          if(++numFound >= maxValues)
            return false;
        }
      }
		}
	}
  return !problemDuringCalculation;
}

bool ReferenceDistanceCalculator::getVecsBetween(
  const ::arma::vec3 & r1,
  const ::arma::vec3 & r2,
  const double cutoff,
  ::std::vector< ::arma::vec3> & outValues,
  const size_t maxValues,
  const unsigned int maxCellMultiples) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

	// Make sure a and b are in the unit cell at the origin
  const ::arma::vec3		dR		= cell.wrapVec(r2) - cell.wrapVec(r1);

	// Get the lattice vectors
  const ::arma::vec3 A(cell.getAVec());
  const ::arma::vec3 B(cell.getBVec());
  const ::arma::vec3 C(cell.getCVec());

  const double safeCutoff = cutoff + sqrt(::arma::dot(dR, dR));

	// Maximum multiple of cell vectors we need to go to
	int A_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(A, B, C, safeCutoff));
	int B_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(B, A, C, safeCutoff));
	int C_max = (int)ceil(getNumPlaneRepetitionsToBoundSphere(C, A, B, safeCutoff));

  bool problemDuringCalculation = false;
  problemDuringCalculation |= capMultiples(A_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(B_max, maxCellMultiples);
  problemDuringCalculation |= capMultiples(C_max, maxCellMultiples);

  const double cutoffSq = cutoff * cutoff;
  ::arma::vec3 outVec;
  size_t numFound = 0;
  ::arma::vec3 r;
	for(int a = -A_max; a <= A_max; ++a)
	{
		for(int b = -B_max; b <= B_max; ++b)
		{
			for(int c = -C_max; c <= C_max; ++c)
			{
        r = a * A + b * B + c * C + dR;

				if(::arma::dot(r, r) < cutoffSq)
        {
          outValues.push_back(r);
          if(++numFound >= maxValues)
            return false;
        }
      }
		}
	}
  return !problemDuringCalculation;
}

bool ReferenceDistanceCalculator::isValid() const
{
  return myStructure.getUnitCell() != NULL;
}

double ReferenceDistanceCalculator::getNumPlaneRepetitionsToBoundSphere(
  const ::arma::vec3 & boundDir,
  const ::arma::vec3 & planeVecA,
  const ::arma::vec3 & planeVecB,
	const double radius) const
{
	// The vector normal to the plane
  const ::arma::vec3 normal = ::arma::cross(planeVecA, planeVecB);
  const double unitCellVolume = myStructure.getUnitCell()->getVolume();

  return radius / unitCellVolume * ::std::sqrt(::arma::dot(normal, normal));
}

}
}
