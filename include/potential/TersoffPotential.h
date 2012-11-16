/*
 * TersoffPotential.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef TERSOFF_POTENTIAL_H
#define TERSOFF_POTENTIAL_H

// DEFINES ////////////////////////////////////////////////


// INCLUDES ///////////////////////////////////////////////
#include <vector>

#include <boost/foreach.hpp>

#include <armadillo>

#include "common/Constants.h"


// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace potential {

template<typename FloatType = double>
class TersoffPotential :
	public IPotential,
	public IParameterisable
{
public:

  typedef TersoffPotentialData<FloatType>					      DataTyp;

  TersoffPotential(
    const FloatType   a,        // eV
    const FloatType   b,        // eV
    const FloatType   lambda1,  // A^-1
    const FloatType   lambda2,  // A^-1
    const FloatType   alpha,
    const FloatType   beta,
    const FloatType   n,
    const FloatType   c,
    const FloatType   d,
    const FloatTyp    h,
    const FloatType   lambda3,  // A^-1
    const FloatType   r,        // A
    const FloatType   d         // A
    );

  virtual TersoffPotentialData<FloatType> *
    generatePotentialData(const sstbx::common::Structure & structure) const;

	virtual bool evalPotential(
		const sstbx::common::Structure & structure,
		TersoffPotentialData<FloatType> & potentialData) const;

private:

  void initParams();

  FloatType myA;
  FloatType myB;
  FloatType myLambda1;
  FloatType myLambda2;
  FloatType myAlpha;
  FloatType myBeta;
  FloatType myN;
  FloatType myC;
  FloatType myD;
  FloatType myH;
  FloatType myLambda3;
  FloatType myR;
  FloatType myD;

  FloatType myCutoff;
  FloatType myRMinusD;

};

// IMPLEMENTATION /////////////////////////////////////

TersoffPotential(
  const FloatType   a,        // eV
  const FloatType   b,        // eV
  const FloatType   lambda1,  // A^-1
  const FloatType   lambda2,  // A^-1
  const FloatType   alpha,
  const FloatType   beta,
  const FloatType   n,
  const FloatType   c,
  const FloatType   d,
  const FloatTyp    h,
  const FloatType   lambda3,  // A^-1
  const FloatType   r,        // A
  const FloatType   d         // A
  ):
myA(a),
myB(b),
myLambda1(lambda1),
myLambda2(lambda2),
myAlpha(alpha),
myBeta(beta),
myN(n),
myC(c),
myD(d),
myH(h),
myLambda3(lambda3),
myR(r),
myD(d)
{
  initParams();
}

template <typename FloatType>
TersoffPotentialData<FloatType> *
TersoffPotential<FloatType>::generatePotentialData(const sstbx::common::Structure & structure) const
{
  return new TersoffPotentialData<FloatType>(structure);
}

template <typename FloatType>
bool TersoffPotential<FloatType>::evalPotential(
		const sstbx::common::Structure & structure,
		TersoffPotentialData<FloatType> & data) const
{
  /**
  /* NOTE: This is a slow initial implementation and may be optimised later if necessary!
  /* For the time being it is being written for correctness, not speed.
  /*
  /**/

  using arma::dot;
  using std::cos;
  using std::sqrt;
  using std::vector;


  typedef arma::Col<FloatType> vec;

  vec rI, rJ, rIJ;    // Position and displacement vectors
  FloatType dRIJ;     // Displacement distance
  vector<vec> imageVectors;
  FloatType fC, fR;      // Repulsive and attractive function values
  FloatType ddr1, ddr2, ddr3, ddrf_C;



  // Loop over all particle pairs, including self
  for(size_t i = 0; i < data.numParticles - 1; ++i)
  {
    rI = data.pos[i];
    for(size_t j = i; i < data.numParticles; ++j)
    {
      rJ = data.pos[j];
      
      // Get all the image vectors rIJ
      imageVectors.clear();
			uc.getAllVectorsWithinCutoff(posI, posJ, myCutoff, imageVectors);

      BOOST_FOREACH(rIJ, imageVectors)
      {
        dRIJ = sqrt(dot(rIJ, rIJ));

        if(dRIJ < myRMinusD)
        {
          fC      = 1;
          ddrf_C  = 0;
        }
        else
        {
          fC = 0.5 - 0.5 * sin(Constants::PI * (dRIJ - myR) / myD);
          ddrf_C = -0.25 * Constants::PI * cos(Constants::PI * (dRIJ - myR) / myD);
        }

        // Calculate the repulsive and attractive parts
        fR = myA * exp(-myLambda1 * dRIJ);
        fA = -myB * exp(-myLambda2 * dRIJ);

        ddr1 = fC * myLambda1 * fR - fR * ddfC;
        ddr2 = /*gamma_ij*/ fC * myLambda2 * fA + fA * ddrF_C;
        //ddr1 = fC * myLambda2 * fR + fR * myLambda1 * fC;
        //ddr2 = fC * myLambda2 * fR + fR * myLambda1 * fC;


      }
    }
  }

}

template <typename FloatType>
void TersoffPotential<FloatType>::initParams()
{
  myCutoff = myR + myD;
  myRMinusD = myR - myD;
}

}}

#endif /* TERSOFF_POTENTIAL_H */
