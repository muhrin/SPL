/*
 * SimplePairPotentialData.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "potential/SimplePairPotential.h"

#include <memory>

#include "common/DistanceCalculator.h"
#include "common/UnitCell.h"

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace potential {

// Using 0.5 prefactor as 2^(1/6) s is the equilibrium separation of the centres.
// i.e. the diameter
const double SimplePairPotential::RADIUS_FACTOR = 0.5 * ::std::pow(2, 1.0/6.0);

const double SimplePairPotential::MIN_SEPARATION_SQ = 1e-20;


SimplePairPotential::SimplePairPotential(
  common::AtomSpeciesDatabase & atomSpeciesDb,
	const size_t 				          numSpecies,
  const SpeciesList &           speciesList,
	const ::arma::mat &		        epsilon,
	const ::arma::mat &		        sigma,
	const double 			            cutoffFactor,
	const ::arma::mat &		        beta,
	const double 			            m,
	const double    			        n,
  const CombiningRule           combiningRule):
	myName("Simple pair potential"),
  myAtomSpeciesDb(atomSpeciesDb),
	myNumSpecies(numSpecies),
  mySpeciesList(speciesList),
	myEpsilon(epsilon),
	mySigma(sigma),
	myBeta(beta),
	myM(m),
	myN(n),
  myCutoffFactor(cutoffFactor),
  myCombiningRule(combiningRule)
{
  applyCombiningRule();
	initCutoff(myCutoffFactor);
  updateSpeciesDb();
}


void SimplePairPotential::initCutoff(double cutoff)
{
	// Initialise the cutoff matrices
	rCutoff.set_size(myNumSpecies, myNumSpecies);
	rCutoffSq.set_size(myNumSpecies, myNumSpecies);
	eShift.set_size(myNumSpecies, myNumSpecies);
	fShift.set_size(myNumSpecies, myNumSpecies);

	rCutoff		= cutoff * mySigma;
	rCutoffSq	= arma::pow(rCutoff, 2.0);

	double invRMaxN, invRMaxM;
	for(size_t i = 0; i < myNumSpecies; ++i)
	{
		for(size_t j = 0; j < myNumSpecies; ++j)
		{
			invRMaxN = pow(mySigma(i, j) / rCutoff(i, j), myN) * myBeta(i, j);

			invRMaxM = pow(mySigma(i, j) / rCutoff(i, j), myM);

			eShift(i, j) = 2.0 * myEpsilon(i, j) * (invRMaxM - invRMaxN);

			fShift(i, j) = 2.0 * myEpsilon(i, j) * (myM * invRMaxM / rCutoff(i, j) -
				myN * invRMaxN / rCutoff(i, j));
		}
	}
}


void SimplePairPotential::applyCombiningRule()
{
  if(myCombiningRule == LORENTZ || myCombiningRule == LORENTZ_BERTHELOT)
  {
    // Apply the Lorenz combining rule
	  for(size_t i = 0; i < myNumSpecies - 1; ++i)
	  {
		  for(size_t j = i + 1; j < myNumSpecies; ++j)
		  {
			  mySigma(i, j) = mySigma(j, i) = 0.5 * (mySigma(i, i) + mySigma(j, j));
		  }
	  }
  }
  if(myCombiningRule == BERTHELOT || myCombiningRule == LORENTZ_BERTHELOT)
  {
    // Apply the Berthelot combining rule
	  for(size_t i = 0; i < myNumSpecies - 1; ++i)
	  {
		  for(size_t j = i + 1; j < myNumSpecies; ++j)
		  {
			  myEpsilon(i, j) = myEpsilon(j, i) = std::sqrt(myEpsilon(i, i) * myEpsilon(j, j));
		  }
	  }
  }
  if(myCombiningRule == CUSTOM)
  {
    double sum = 0.0;
    // Apply the Berthelot combining rule
	  for(size_t i = 0; i < myNumSpecies - 1; ++i)
	  {
		  for(size_t j = i + 1; j < myNumSpecies; ++j)
		  {
        sum = myEpsilon(i, i) + myEpsilon(j, j);
			  myEpsilon(i, j) = myEpsilon(j, i) = std::sqrt(16 - sum * sum);
		  }
	  }
  }
}


const ::std::string & SimplePairPotential::getName() const
{
	return myName;
}


size_t SimplePairPotential::getNumParams() const
{
  // 2 (eps/sig) * n * (n + 1) / 2 + n(species) = n(n + 2)
  return (myNumSpecies * (myNumSpecies + 2));
}


::arma::vec SimplePairPotential::getParams() const
{
	::arma::vec params(getNumParams());

  size_t idx = 0;

	// Epsilon
	for(size_t i = 0; i < myNumSpecies; ++i)
	{
		for(size_t j = i; j < myNumSpecies; ++j, ++idx)
		{
			params(idx) = myEpsilon(i, j);
		}
	}
	// Sigma
	for(size_t i = 0; i < myNumSpecies; ++i)
	{
		for(size_t j = i; j < myNumSpecies; ++j, ++idx)
		{
			params(idx) = mySigma(i, j);
		}
	}
  for(size_t i = 0; i < myNumSpecies; ++i, ++idx)
  {
    params(idx) = mySpeciesList[i].ordinal();
  }

	return params;
}


void SimplePairPotential::setParams(const ::arma::vec & params)
{
	if(params.n_rows != getNumParams())
	{
		throw "setParams called with wrong number of parameters";
	}

  size_t idx = 0;

	// Epsilon
	for(size_t i = 0; i < myNumSpecies; ++i)
	{
		for(size_t j = i; j < myNumSpecies; ++j, ++idx)
		{
			myEpsilon(i, j) = params(idx);
		}
	}
  myEpsilon = arma::symmatu(myEpsilon);

	// Sigma
	for(size_t i = 0; i < myNumSpecies; ++i)
	{
		for(size_t j = i; j < myNumSpecies; ++j, ++idx)
		{
			mySigma(i, j) = params(idx);
		}
  }
  mySigma = arma::symmatu(mySigma);

  applyCombiningRule();

  // Initialise the cutoff matrices
  initCutoff(myCutoffFactor);

  // Set the species list
  for(size_t i = 0; i < myNumSpecies; ++i, ++idx)
  {
    mySpeciesList[i] = *::sstbx::common::AtomSpeciesId::values().operator[]((int)params[idx]);
  }

	// Reset the parameter string
	myParamString.clear();
  // Update the species database
  updateSpeciesDb();
}


std::pair<arma::vec, bool>
SimplePairPotential::getParamsFromString(const std::string & str) const
{
  using boost::trim;
  using boost::lexical_cast;
  using boost::bad_lexical_cast;
  using std::string;

  // Set up our tokenizer to split around space and tab
	typedef boost::tokenizer<boost::char_separator<char> > Tok;
	const boost::char_separator<char> sep(" \t");

  std::pair<arma::vec, bool> result;
  result.second = false;

  // Look for parameter indicators
  size_t nPos = str.find("n:");
  size_t ePos = str.find("e:");
  size_t sPos = str.find("s:");

  // Check to see if they were all gound
  if(nPos != string::npos && ePos != string::npos && sPos != string::npos)
  {
    // Try to get n
    bool foundN = false;
    unsigned int nSpecies = 0;
    try
    {
      string nStr = str.substr(nPos + 2, ePos - nPos - 2);
      trim(nStr);
      nSpecies = lexical_cast<unsigned int>(nStr);
      foundN = true;
    }
    catch(const bad_lexical_cast &)
    {
      foundN = true;
    }

    if(foundN)
    { 
      // Calculate the number of parameters for sigma/epsilon
      const unsigned int nTotal = nSpecies * (nSpecies + 1);  // Guaranteed to be even
      const unsigned int nEach  = nTotal / 2;                 // Safe to divide by 2
      result.first.set_size(nTotal);

      // Try to get epsilon
      const string eString = str.substr(ePos + 2, sPos - ePos - 2);
      Tok eToker(eString, sep);

      bool foundE = true;
      unsigned int i = 0;
      for(Tok::const_iterator eIt = eToker.begin();
        i < nEach && eIt != eToker.end();
        ++i, ++eIt)
      {
        try
        {
          result.first(i) = lexical_cast<double>(*eIt);
        }
        catch(const bad_lexical_cast &)
        {
          foundE = false;
          break;
        }
      }
      foundE &= i == nEach;

      if(foundE)
      { 
        // Try to get sigma
        const string sString = str.substr(sPos + 2);
        Tok sToker(sString, sep);

        bool foundS = true;
        unsigned int i = nEach;
        for(Tok::iterator sIt = sToker.begin();
          i < nTotal && sIt != sToker.end();
          ++i, ++sIt)
        {
          try
          {
            result.first(i) = lexical_cast<double>(*sIt);
          }
          catch(const bad_lexical_cast &)
          {
            foundS = false;
            break;
          }
        }
        foundS &= i == nTotal;
        
        if(foundS)
          result.second = true;

      } // end if(foundE)
    } // end if(foundN)
  } // end if n, s, e tokens found
  return result;
}


const ::std::string & SimplePairPotential::getParamString() const
{
	if(myParamString.empty())
	{
		::std::stringstream ss;
		ss << "n: " << myNumSpecies << " e:";
		// Epsilon
		for(size_t i = 0; i < myNumSpecies; ++i)
		{
			for(size_t j = i; j < myNumSpecies; ++j)
			{
				ss << " " << myEpsilon(i, j);
			}
		}
		ss << " s:";
		// Sigma
		for(size_t i = 0; i < myNumSpecies; ++i)
		{
			for(size_t j = i; j < myNumSpecies; ++j)
			{
				ss << " " << mySigma(i, j);
			}
		}
		myParamString = ss.str();
	}
	return myParamString;
}

bool SimplePairPotential::evaluate(const common::Structure & structure, SimplePairPotentialData & data) const
{
	using ::std::vector;

	double rSq;
	double sigmaOModR, invRM, invRN;
	double dE, modR, modF;
  size_t speciesI, speciesJ;  // Species indices
	::arma::vec3 r, f;        // Displacement and force vectors
  ::arma::vec3 posI, posJ;  // Position vectors

	resetAccumulators(data);

	vector< ::arma::vec3> imageVectors;

  const common::DistanceCalculator & distCalc = structure.getDistanceCalculator();
	
  bool problemDuringCalculation = false;

	// Loop over all particle pairs (including self-interaction)
	for(size_t i = 0; i < data.numParticles; ++i)
	{
		speciesI = data.species[i];
    if(speciesI == DataType::IGNORE_ATOM)
      continue;

		posI = data.pos.col(i);

		for(size_t j = i; j < data.numParticles; ++j)
		{
			speciesJ = data.species[j];
      if(speciesJ == DataType::IGNORE_ATOM)
        continue;

			posJ = data.pos.col(j);

      // TODO: Buffer rSqs as getAllVectorsWithinCutoff needs to calculate it anyway!
			imageVectors.clear();
      if(!distCalc.getVecsBetween(posI, posJ, rCutoff(speciesI, speciesJ), imageVectors, MAX_INTERACTION_VECTORS, MAX_CELL_MULTIPLES))
      {
        // We reached the maximum number of interaction vectors so indicate that there was a problem
        problemDuringCalculation = true;
      }

      BOOST_FOREACH(r, imageVectors)
			{			
				// Get the distance squared
				rSq = dot(r, r);

				// Check that distance isn't near the 0 as this will cause near-singular values
				if(rSq > MIN_SEPARATION_SQ)
				{
					modR = sqrt(rSq);

					sigmaOModR = mySigma(speciesI, speciesJ) / modR;

					invRM = pow(sigmaOModR, myM);
					invRN = pow(sigmaOModR, myN) * myBeta(speciesI, speciesJ);

					// Calculate the energy delta
					dE = 2.0 * myEpsilon(speciesI, speciesJ) * (invRM - invRN) -
						eShift(speciesI, speciesJ) + (modR - rCutoff(speciesI, speciesJ)) * fShift(speciesI, speciesJ);

					// Magnitude of the force
					modF = 2.0 *  myEpsilon(speciesI, speciesJ) *
						(myM * invRM - myN * invRN) / modR - fShift(speciesI, speciesJ);
					f = modF / modR * r;

					// Make sure we get energy/force correct for self-interaction
					if(i != j)
					{
						f *= 2.0;
						dE *= 2.0;
					}

					// Update system values
					// energy
					data.internalEnergy += dE;
					// force
					data.forces.col(i) -= f;
					if(i != j)
						data.forces.col(j) += f;
					
					// stress, diagonal is element wise multiplication of force and position
					// vector components
					data.stressMtx.diag() += f % r;
					
					data.stressMtx(1, 2) += 0.5 * (f(1)*r(2)+f(2)*r(1));
					data.stressMtx(2, 0) += 0.5 * (f(2)*r(0)+f(0)*r(2));
					data.stressMtx(0, 1) += 0.5 * (f(0)*r(1)+f(1)*r(0));

				}
			}
		}
	}

	// Symmetrise stress matrix
	data.stressMtx(2, 1) = data.stressMtx(1, 2);
	data.stressMtx(0, 2) = data.stressMtx(2, 0);
	data.stressMtx(1, 0) = data.stressMtx(0, 1);

  const common::UnitCell * const unitCell = structure.getUnitCell();

  if(unitCell)
  {
	  // And convert to absoloute values
	  const double invVolume = 1.0 / unitCell->getVolume();
	  data.stressMtx *= invVolume;
  }

  // Completed successfully
  return !problemDuringCalculation;
}


::boost::optional<double>
SimplePairPotential::getPotentialRadius(const ::sstbx::common::AtomSpeciesId::Value id) const
{
  ::boost::optional<double> radius;
  for(size_t i = 0; i < mySpeciesList.size(); ++i)
  {
    if(mySpeciesList[i] == id)
    {
      radius.reset(RADIUS_FACTOR * mySigma(i, i));
      break;
    }
  }
  return radius;
}


::boost::shared_ptr<IPotentialEvaluator>
SimplePairPotential::createEvaluator(const sstbx::common::Structure & structure) const
{
  // Build the data from the structure
  ::std::auto_ptr<SimplePairPotentialData> data(new SimplePairPotentialData(structure, mySpeciesList));

  // Create the evaluator
  return ::boost::shared_ptr<IPotentialEvaluator>(new Evaluator(*this, structure, data));
}



void SimplePairPotential::resetAccumulators(SimplePairPotentialData & data) const
{
	data.internalEnergy = 0.0;
	data.forces.zeros();
	data.stressMtx.zeros();
}

void SimplePairPotential::updateSpeciesDb()
{
  BOOST_FOREACH(common::AtomSpeciesId::Value species, mySpeciesList)
  {
    myAtomSpeciesDb.setRadius(species, *getPotentialRadius(species));
  }
}

}
}
