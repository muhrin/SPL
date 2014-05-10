/*
 * LennardJones.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/LennardJones.h"

#include <memory>

#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/DistanceCalculator.h"
#include "spl/common/UnitCell.h"
#include "spl/utility/IndexingEnums.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace potential {

// Using 2^(1/6) s is the equilibrium separation of the centres.
const double LennardJones::EQUILIBRIUM_FACTOR = ::std::pow(2, 1.0 / 6.0);
const double LennardJones::MIN_SEPARATION_SQ = 1e-20;

unsigned int
LennardJones::numParams(const unsigned int numSpecies)
{
  return 0;
}

LennardJones::LennardJones(const SpeciesList & speciesList,
    const ::arma::mat & epsilon, const ::arma::mat & sigma,
    const double cutoffFactor, const ::arma::mat & beta, const double m,
    const double n, const CombiningRule::Value combiningRule) :
    myName("Simple pair potential"), myNumSpecies(speciesList.size()), mySpeciesList(
        speciesList), myEpsilon(epsilon), mySigma(sigma), myBeta(beta), myCutoffFactor(
        cutoffFactor), myM(m), myN(n), myCombiningRule(combiningRule)
{
  SSLIB_ASSERT(myNumSpecies == myEpsilon.n_rows);
  SSLIB_ASSERT(myEpsilon.is_square());
  SSLIB_ASSERT(myNumSpecies == mySigma.n_rows);
  SSLIB_ASSERT(mySigma.is_square());
  SSLIB_ASSERT(myNumSpecies == myBeta.n_rows);
  SSLIB_ASSERT(myBeta.is_square());

  init();
}

void
LennardJones::init()
{
  applyCombiningRule();
  initCutoff();
  updateEquilibriumSeparations();
}

void
LennardJones::initCutoff()
{
  // Initialise the cutoff matrices
  rCutoff.set_size(myNumSpecies, myNumSpecies);
  rCutoffSq.set_size(myNumSpecies, myNumSpecies);
  eShift.set_size(myNumSpecies, myNumSpecies);
  fShift.set_size(myNumSpecies, myNumSpecies);

  rCutoff = myCutoffFactor * mySigma;
  rCutoffSq = rCutoff % rCutoff;

  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j)
    {
      eShift(i, j) = U(i, j, rCutoff(i, j));
      fShift(i, j) = F(i, j, rCutoff(i, j));
    }
  }
  // Copy over upper triangular
  eShift = ::arma::trimatu(eShift);
  fShift = ::arma::trimatl(fShift);
}

void
LennardJones::applyCombiningRule()
{
  applyEnergyRule(myEpsilon, myCombiningRule);
  applySizeRule(mySigma, myCombiningRule);
}

const ::std::string &
LennardJones::getName() const
{
  return myName;
}

size_t
LennardJones::getNumParams() const
{
  // epsilon: n(n + 1) / 2
  // sigma: n(n + 1) / 2
  // beta: n(n + 1) / 2
  // m-n: 2
  return 3 * myNumSpecies * (myNumSpecies + 1) / 2 + 2;
}

IParameterisable::PotentialParams
LennardJones::getParams() const
{
  IParameterisable::PotentialParams params(getNumParams());

  size_t idx = 0;

  // Epsilon
  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j)
      params[idx++] = myEpsilon(i, j);
  }
  // Sigma
  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j)
      params[idx++] = mySigma(i, j);
  }
  // Beta
  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j)
      params[idx++] = myBeta(i, j);
  }
  // M-N
  params[idx++] = myM;
  params[idx++] = myN;

  return params;
}

void
LennardJones::setParams(const IParameterisable::PotentialParams & params)
{
  SSLIB_ASSERT_MSG(params.size() == getNumParams(),
      "Called setParams with wrong number of parameters");

  size_t idx = 0;

  // Epsilon
  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j, ++idx)
      myEpsilon(i, j) = params[idx];
  }
  myEpsilon = arma::symmatu(myEpsilon);

  // Sigma
  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j, ++idx)
      mySigma(i, j) = params[idx];
  }
  mySigma = arma::symmatu(mySigma);

  // Beta
  for(size_t i = 0; i < myNumSpecies; ++i)
  {
    for(size_t j = i; j < myNumSpecies; ++j, ++idx)
      myBeta(i, j) = params[idx];
  }
  myBeta = arma::symmatu(myBeta);

  myM = params[idx++];
  myN = params[idx++];

  // Initialise everything with the new params
  init();
}

bool
LennardJones::updateSpeciesDb(
    common::AtomSpeciesDatabase * const speciesDb) const
{
  SSLIB_ASSERT(speciesDb);

  for(int i = 0; i < static_cast< int>(myNumSpecies); ++i)
  {
    speciesDb->setRadius(mySpeciesList[i],
        *getPotentialRadius(mySpeciesList[i]));
    for(int j = i; j < static_cast< int>(myNumSpecies); ++j)
    {
      speciesDb->setSpeciesPairDistance(
          common::SpeciesPair(mySpeciesList[i], mySpeciesList[j]),
          *getSpeciesPairDistance(mySpeciesList[i], mySpeciesList[j]));
    }
  }
  return true;
}

bool
LennardJones::evaluate(const common::Structure & structure,
    SimplePairPotentialData & data) const
{
  using namespace utility::cart_coords_enum;
  using ::std::vector;
  using ::std::pow;
  using ::std::sqrt;

  double rSq;
  double prefactor;
  double sigmaOModR, invRM, invRN;
  double dE, modR, modF, interaction;
  size_t speciesI, speciesJ; // Species indices
  ::arma::vec3 f; // Displacement and force vectors
  ::arma::vec3 posI, posJ; // Position vectors

  resetAccumulators(data);

  vector< ::arma::vec3> imageVectors;

  const common::DistanceCalculator & distCalc =
      structure.getDistanceCalculator();

  bool problemDuringCalculation = false;

  // Loop over all particle pairs (including self-interaction)
  for(size_t i = 0; i < data.numParticles; ++i)
  {
    if(data.species[i] == DataType::IGNORE_ATOM)
      continue;
    else
      speciesI = static_cast< size_t>(data.species[i]);
    posI = data.pos.col(i);

    for(size_t j = i; j < data.numParticles; ++j)
    {
      if(data.species[j] == DataType::IGNORE_ATOM)
        continue;
      else
        speciesJ = static_cast< size_t>(data.species[j]);
      posJ = data.pos.col(j);

      imageVectors.clear();
      if(!distCalc.getVecsBetween(posI, posJ, rCutoff(speciesI, speciesJ),
          imageVectors, MAX_INTERACTION_VECTORS, MAX_CELL_MULTIPLES))
      {
        // We reached the maximum number of interaction vectors so indicate that there was a problem
        problemDuringCalculation = true;
      }

      // Used as a prefactor depending if the particles i and j are in fact the same
      interaction = (i == j) ? 0.5 : 1.0;
      prefactor = 4.0 * myEpsilon(speciesI, speciesJ);

      BOOST_FOREACH(const ::arma::vec & r, imageVectors)
      {
        // Get the distance squared
        rSq = dot(r, r);

        // Check that distance isn't near the 0 as this will cause near-singular values
        if(rSq > MIN_SEPARATION_SQ)
        {
          modR = sqrt(rSq);

          sigmaOModR = mySigma(speciesI, speciesJ) / modR;

          invRN = pow(sigmaOModR, myN);
          // Deal with special case where N is 2 times M avoiding second pow call
          invRM = myM == 2.0 * myN ? invRN * invRN : pow(sigmaOModR, myM);
          invRN *= myBeta(speciesI, speciesJ);

          // Calculate the energy delta
          dE = prefactor * (invRM - invRN) - eShift(speciesI, speciesJ)
              + (modR - rCutoff(speciesI, speciesJ))
                  * fShift(speciesI, speciesJ);

          // Magnitude of the force
          modF = prefactor * (myM * invRM - myN * invRN) / modR
              - fShift(speciesI, speciesJ);
          f = modF / modR * r;

          // Make sure we get energy/force correct for self-interaction
          f *= interaction;
          dE *= interaction;

          // Update system values
          // energy
          data.internalEnergy += dE;
          // force
          data.forces.col(i) -= f;
          if(i != j)
            data.forces.col(j) += f;

          // stress, diagonal is element wise multiplication of force and position
          // vector components
          data.stressMtx.diag() -= f % r;

          data.stressMtx(Y, Z) -= 0.5 * (f(Y) * r(Z) + f(Z) * r(Y));
          data.stressMtx(Z, X) -= 0.5 * (f(Z) * r(X) + f(X) * r(Z));
          data.stressMtx(X, Y) -= 0.5 * (f(X) * r(Y) + f(Y) * r(X));
        }
      }
    }
  }

  // Symmetrise stress matrix
  data.stressMtx(2, 1) = data.stressMtx(1, 2);
  data.stressMtx(0, 2) = data.stressMtx(2, 0);
  data.stressMtx(1, 0) = data.stressMtx(0, 1);

  // Now balance forces
  // (do sum of values for each component and divide by number of particles)
  f = sum(data.forces, 1) / static_cast< double>(data.numParticles);
  data.forces.row(X) -= f(Y);
  data.forces.row(Y) -= f(X);
  data.forces.row(Z) -= f(Z);

  // Convert stress matrix to absolute values
  const common::UnitCell * const unitCell = structure.getUnitCell();
  if(unitCell)
  {
    const double invVolume = 1.0 / unitCell->getVolume();
    data.stressMtx *= invVolume;
  }

  // Completed successfully
  return !problemDuringCalculation;
}

::boost::optional< double>
LennardJones::getPotentialRadius(
    const ::spl::common::AtomSpeciesId::Value id) const
{
  ::boost::optional< double> radius = getSpeciesPairDistance(id, id);
  if(radius)
    *radius *= 0.5;

  return radius;
}

::boost::optional< double>
LennardJones::getSpeciesPairDistance(common::AtomSpeciesId::Value s1,
    common::AtomSpeciesId::Value s2) const
{
  if(s1 < s2)
    ::std::swap(s1, s2);

  ::boost::optional< double> dist;
  int idx1 = -1, idx2 = -1;
  for(size_t i = 0; i < mySpeciesList.size(); ++i)
  {
    if(mySpeciesList[i] == s1)
      idx1 = i;
    if(mySpeciesList[i] == s2)
      idx2 = i;

    if(idx1 != -1 && idx2 != -1)
      break;
  }

  if(idx1 != -1 && idx2 != -1)
    dist.reset(myEquilibriumSeps(idx1, idx2));

  return dist;
}

::boost::shared_ptr< IPotentialEvaluator>
LennardJones::createEvaluator(const spl::common::Structure & structure) const
{
  // Build the data from the structure
  ::std::auto_ptr< SimplePairPotentialData> data(
      new SimplePairPotentialData(structure, mySpeciesList));

  // Create the evaluator
  return ::boost::shared_ptr< IPotentialEvaluator>(
      new Evaluator(*this, structure, data));
}

IParameterisable *
LennardJones::getParameterisable()
{
  return this;
}

void
LennardJones::resetAccumulators(SimplePairPotentialData & data) const
{
  data.internalEnergy = 0.0;
  data.forces.zeros();
  data.stressMtx.zeros();
}

void
LennardJones::updateEquilibriumSeparations()
{
  const size_t numSpecies = mySpeciesList.size();
  myEquilibriumSeps.set_size(numSpecies, numSpecies);

  for(size_t i = 0; i < numSpecies; ++i)
  {
    for(size_t j = i; j < numSpecies; ++j)
      myEquilibriumSeps(i, j) = EQUILIBRIUM_FACTOR * mySigma(i, i);
    // TODO: Use root finder to solve for the F(r) = 0 point
  }
  // Copy over upper triangular
  myEquilibriumSeps = ::arma::symmatu(myEquilibriumSeps);
}

}
}
