/*
 * LennardJones.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/LennardJones.h"

#include <memory>

#include <boost/algorithm/string.hpp>

#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/DistanceCalculator.h"
#include "spl/common/UnitCell.h"
#include "spl/utility/IndexingEnums.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace potential {

// Using 2^(1/6) s is the equilibrium separation of the centres.
const double LennardJones::MIN_SEPARATION_SQ = 1e-20;

void
LennardJones::init()
{
  applyCombiningRule();
}

void
LennardJones::applyCombiningRule()
{
  // Get all the interactions that are between the same species e.g. A~A
  std::vector< Interactions::const_iterator> species;
  for(Interactions::const_iterator it = myInteractions.begin(), end =
      myInteractions.end(); it != end; ++it)
  {
    if(it->first.first() == it->first.second())
      species.push_back(it);

  }

  for(size_t i = 0; i < species.size(); ++i)
  {
    const Params & pI = species[i]->second;
    for(size_t j = i + 1; j < species.size(); ++j)
    {
      const Params & pJ = species[j]->second;

      // Can only apply the rule if they both have their other parameters the same
      if(pI.m == pJ.m && pI.n == pJ.n)
      {
        const Interactions::const_iterator it = myInteractions.find(
            SpeciesPair(species[i]->first.first(), species[j]->first.first()));

        if(it == myInteractions.end())
        {
          getParams(applyRule(myEpsilonCombining, pI.epsilon, pJ.epsilon),
              applyRule(mySigmaCombining, pI.sigma, pJ.sigma), pI.m, pI.n,
              pI.cutoff / pI.sigma);
        }
      }
    }
  }
}

std::vector< std::string>
LennardJones::getParams() const
{
  using boost::lexical_cast;
  using std::string;

  std::vector< string> params;
  BOOST_FOREACH(Interactions::const_reference inter, myInteractions)
  {
    params.push_back(lexical_cast< string>(inter.first));
    params.push_back(lexical_cast< string>(inter.second.epsilon));
    params.push_back(lexical_cast< string>(inter.second.sigma));
    params.push_back(lexical_cast< string>(inter.second.m));
    params.push_back(lexical_cast< string>(inter.second.n));
  }

  params.push_back(lexical_cast< string>(myEpsilonCombining));
  params.push_back(lexical_cast< string>(mySigmaCombining));

  return params;
}

bool
LennardJones::setParams(const std::vector< std::string> & params,
    std::string * const errorMsg)
{
  using boost::lexical_cast;
  using std::string;

  typedef std::vector< std::string>::const_iterator ParamsIter;

  if(params.empty())
  {
    myInteractions.clear();
    return true;
  }

  Interactions newInteractions;

  const size_t numCombining = params.size() % 6;
  const size_t numInteractions = params.size() / 6;
  if(numCombining > 2)
  {
    if(errorMsg)
      *errorMsg = "Wrong number of parameters given.";
    return false;
  }

  SpeciesPair species;
  double paramsList[5];
  for(size_t i = 0; i < numInteractions; ++i)
  {
    ParamsIter it = params.begin() + (i * 6);
    try
    {
      species = lexical_cast< SpeciesPair>(*it);
      ++it;
    }
    catch(const boost::bad_lexical_cast & /*e*/)
    {
      if(errorMsg)
        *errorMsg = "Malformatted species pair: " + *it;
      return false;
    }
    try
    {
      // Convert the strings to doubles
      for(size_t j = 0; j < 5; ++j, ++it)
        paramsList[j] = lexical_cast< double>(*it);
    }
    catch(const boost::bad_lexical_cast & /*e*/)
    {
      if(errorMsg)
      {
        *errorMsg = "Potential parameter \'" + *it
            + "' contains invalid characters.";
      }
      return false;
    }
    newInteractions[SpeciesPair(species)] = getParams(paramsList[0],
        paramsList[1], paramsList[2], paramsList[3], paramsList[4]);
  }

  // Do the combining rules
  CombiningRule::Value epsilonCombining, sigmaCombining;
  if(numCombining > 0)
  {
    ParamsIter it = params.begin() + (numInteractions * 6);
    try
    {
      epsilonCombining = lexical_cast< CombiningRule::Value>(*it);
      if(numCombining > 1)
        sigmaCombining = lexical_cast< CombiningRule::Value>(*++it);
    }
    catch(const boost::bad_lexical_cast & e)
    {
      if(errorMsg)
        *errorMsg = (*it) + " is not a valid combining rule.";
      return false;
    }
  }

  myInteractions = newInteractions;
  myEpsilonCombining = epsilonCombining;
  mySigmaCombining = sigmaCombining;
  // Initialise everything with the new params
  init();

  return true;
}

bool
LennardJones::updateSpeciesDb(
    common::AtomSpeciesDatabase * const speciesDb) const
{
  SSLIB_ASSERT(speciesDb);

  BOOST_FOREACH(Interactions::const_reference inter, myInteractions)
    speciesDb->setSpeciesPairDistance(inter.first,
        equilibriumSeparation(inter.second));

  return true;
}

bool
LennardJones::evaluate(const common::Structure & structure,
    PotentialData & data) const
{
  using namespace utility::cart_coords_enum;
  using std::vector;
  using std::pow;
  using std::sqrt;

  const size_t numParticles = structure.getNumAtoms();
  if(data.forces.n_rows != 3 || data.forces.n_cols != numParticles)
    data.forces.set_size(3, numParticles);
  double rSq, modR, modF, selfInteraction;
// Displacement and force vectors
  arma::vec3 f;
// Position vectors
  arma::vec3 posI, posJ;
// Energy and force scalars
  std::pair< double, double> energyForce;

// Get the species of all the atoms
  vector< std::string> species;
  structure.getAtomSpecies(std::back_inserter(species));

  vector< arma::vec3> imageVectors;

  const common::DistanceCalculator & distCalc =
      structure.getDistanceCalculator();

  bool problemDuringCalculation = false;
  Interactions::const_iterator interaction;

// Loop over all particle pairs (including self-interaction)
  for(size_t i = 0; i < numParticles; ++i)
  {
    posI = structure.getAtom(i).getPosition();

    for(size_t j = i; j < numParticles; ++j)
    {
      // TODO: Set species
      interaction = myInteractions.find(SpeciesPair(species[i], species[j]));
      if(interaction == myInteractions.end())
        continue;

      const Params & params = interaction->second;
      posJ = structure.getAtom(j).getPosition();

      imageVectors.clear();
      if(!distCalc.getVecsBetween(posI, posJ, params.cutoff, imageVectors,
          MAX_INTERACTION_VECTORS, MAX_CELL_MULTIPLES))
      {
        // We reached the maximum number of interaction vectors so indicate that there was a problem
        problemDuringCalculation = true;
      }

      // Used as a prefactor depending if the particles i and j are in fact the same
      selfInteraction = (i == j) ? 0.5 : 1.0;
      BOOST_FOREACH(const arma::vec & r, imageVectors)
      {
        // Get the distance squared
        rSq = dot(r, r);

        // Check that distance isn't near the 0 as this will cause near-singular values
        if(rSq > MIN_SEPARATION_SQ)
        {
          modR = sqrt(rSq);
          energyForce = evaluate(modR, params);

          f = energyForce.second / modR * r;
          // Make sure we get energy/force correct for self-interaction
          f *= selfInteraction;

          // Update system values
          // energy
          data.internalEnergy += selfInteraction * energyForce.first;
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
  f = sum(data.forces, 1) / static_cast< double>(numParticles);
  data.forces.row(X) -= f(Y);
  data.forces.row(Y) -= f(X);
  data.forces.row(Z) -= f(Z);

// Convert stress matrix to absolute values
  const common::UnitCell * const unitCell = structure.getUnitCell();
  if(unitCell)
    data.stressMtx *= 1.0 / unitCell->getVolume();

// Completed successfully
  return !problemDuringCalculation;
}

std::pair< LennardJones::Interactions::const_iterator, bool>
LennardJones::addInteraction(const SpeciesPair & species, const double epsilon,
    const double sigma, const double m, const double n,
    const double cutoffFactor)
{
  if(cutoffFactor == 0.0)
    return std::pair< Interactions::const_iterator, bool>(myInteractions.end(),
        false);

  const std::pair< LennardJones::Interactions::const_iterator, bool> ret =
      myInteractions.insert(
          std::make_pair(species,
              getParams(epsilon, sigma, m, n, cutoffFactor)));
  init();

  return ret;
}

std::pair< double, double>
LennardJones::evaluate(const double r, const Params & params) const
{
  using std::pow;

  const double sr = params.sigma / r;

  const double sr2n = pow(sr, params.n);
// Deal with special case where N is two times M avoiding second pow call
  const double sr2m =
      params.m == 2.0 * params.n ? sr2n * sr2n : pow(sr, params.m);

  std::pair< double, double> energyForce;
// Calculate the energy delta
  energyForce.first = 4.0 * params.epsilon * (sr2m - sr2n) - params.energyShift
      + (r - params.cutoff) * params.forceShift;

// Magnitude of the force
  energyForce.second = 4.0 * params.epsilon
      * (params.m * sr2m - params.n * sr2n) / r - params.forceShift;

  return energyForce;
}

boost::optional< double>
LennardJones::getSpeciesPairDistance(const SpeciesPair & pair) const
{
  const Interactions::const_iterator it = myInteractions.find(pair);
  if(it != myInteractions.end())
    return equilibriumSeparation(it->second);

  return boost::optional< double>();
}

boost::shared_ptr< IPotentialEvaluator>
LennardJones::createEvaluator(const spl::common::Structure & structure) const
{
// Build the data from the structure
  UniquePtr< PotentialData>::Type data(new PotentialData());

// Create the evaluator
  return boost::shared_ptr< IPotentialEvaluator>(
      new Evaluator(*this, structure, data));
}

IParameterisable *
LennardJones::getParameterisable()
{
  return this;
}

double
LennardJones::equilibriumSeparation(const Params & params) const
{
// TODO: Update with cutoff version
  return std::pow(params.m / params.n, 1.0 / (params.m - params.n))
      * params.sigma;
}

LennardJones::Params
LennardJones::getParams(const double epsilon, const double sigma,
    const double m, const double n, const double cutoffFactor) const
{
  Params params;
  params.epsilon = epsilon;
  params.sigma = sigma;
  params.m = m;
  params.n = n;
  params.cutoff = cutoffFactor * sigma;

// Now calculate the energy and force shifts
  params.energyShift = 0.0;
  params.forceShift = 0.0;
  std::pair< double, double> energyForce = evaluate(params.cutoff, params);
  params.energyShift = energyForce.first;
  params.forceShift = energyForce.second;

  return params;
}

}
}
