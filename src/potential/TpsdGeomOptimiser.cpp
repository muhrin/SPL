/*
 * TpsdGeomOptimiser.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/TpsdGeomOptimiser.h"

#include <sstream>

#include "spl/SSLib.h"
#include "spl/common/UnitCell.h"
#include "spl/potential/OptimisationSettings.h"

#define TPSD_GEOM_OPTIMISER_DEBUG (SSLIB_DEBUG & 0)
//#define TPSD_GEOM_OPTIMISER_TIMING

#if TPSD_GEOM_OPTIMISER_DEBUG
#  include <sstream>
#  include "spl/common/AtomSpeciesDatabase.h"
#  include "spl/io/ResourceLocator.h"
#  include "spl/io/ResReaderWriter.h"
#endif

#ifdef TPSD_GEOM_OPTIMISER_TIMING
#  include <ctime>
#endif

// NAMESPACES ////////////////////////////////
namespace spl {
namespace potential {

#if TPSD_GEOM_OPTIMISER_DEBUG
class TpsdGeomOptimiserDebugger
{
public:
  const static unsigned int SAVE_EVERY_N_STEPS = 1000;
  TpsdGeomOptimiserDebugger()
  {
  }

  void
  postOptStepDebugHook(common::Structure & structure,
      const unsigned int step) const
  {
    if(step % SAVE_EVERY_N_STEPS == 0)
    {
      std::stringstream ss;
      ss << structure.getName() << "-" << step << "-" << "debug.res";
      io::ResourceLocator loc;
      if(loc.set(ss.str()))
      myWriter.writeStructure(structure, loc);
    }
  }

private:
  const common::AtomSpeciesDatabase mySpeciesDb;
  io::ResReaderWriter myWriter;
};
#endif

// CONSTANTS ////////////////////////////////////////////////

const unsigned int TpsdGeomOptimiser::DEFAULT_MAX_ITERATIONS = 10000;
const double TpsdGeomOptimiser::DEFAULT_ENERGY_TOLERANCE = 5e-11;
const double TpsdGeomOptimiser::DEFAULT_FORCE_TOLERANCE = 3e-4;
const double TpsdGeomOptimiser::DEFAULT_STRESS_TOLERANCE = 5e-5;
const unsigned int TpsdGeomOptimiser::CHECK_CELL_EVERY_N_STEPS = 20;
const double TpsdGeomOptimiser::CELL_MIN_NORM_VOLUME = 0.02;
const double TpsdGeomOptimiser::CELL_MAX_ANGLE_SUM = 360.0;
const double TpsdGeomOptimiser::DEFAULT_MAX_DELTA_POS_FACTOR = 0.5;
const double TpsdGeomOptimiser::DEFAULT_MAX_DELTA_LATTICE_FACTOR = 0.1;
static const double INITIAL_STEPSIZE = 0.001;

// IMPLEMENTATION //////////////////////////////////////////////////////////

TpsdGeomOptimiser::TpsdGeomOptimiser(PotentialPtr potential) :
    myPotential(potential)
{
}

IPotential *
TpsdGeomOptimiser::getPotential()
{
  return myPotential.get();
}

const IPotential *
TpsdGeomOptimiser::getPotential() const
{
  return myPotential.get();
}

OptimisationOutcome
TpsdGeomOptimiser::optimise(spl::common::Structure & structure,
    const OptimisationSettings & options) const
{
  OptimisationData optData;
  return optimise(structure, optData, options);
}

OptimisationOutcome
TpsdGeomOptimiser::optimise(spl::common::Structure & structure,
    OptimisationData & data, const OptimisationSettings & options) const
{
  UniquePtr< IPotentialEvaluator>::Type evaluator =
      myPotential->createEvaluator(structure);

  common::UnitCell * const unitCell = structure.getUnitCell();
  OptimisationSettings localSettings = options;

  if(!localSettings.maxIter)
    localSettings.maxIter = DEFAULT_MAX_ITERATIONS;
  if(!localSettings.pressure)
    localSettings.pressure = arma::zeros< arma::mat>(3, 3);
  if(!localSettings.optimisationType)
    localSettings.optimisationType =
        OptimisationSettings::Optimise::ATOMS_AND_LATTICE;
  if(!localSettings.stressTol)
    localSettings.stressTol = DEFAULT_STRESS_TOLERANCE;

  OptimisationOutcome outcome;
  if(unitCell)
    outcome = optimise(structure, *unitCell, data, *evaluator, localSettings);
  else
    outcome = optimise(structure, data, *evaluator, localSettings);

  data.saveToStructure(structure);

  return outcome;
}

OptimisationOutcome
TpsdGeomOptimiser::optimise(common::Structure & structure,
    OptimisationData & optimisationData, IPotentialEvaluator & evaluator,
    const OptimisationSettings & settings) const
{
  SSLIB_ASSERT(settings.maxIter.is_initialized());

#ifdef TPSD_GEOM_OPTIMISER_TIMING
  const clock_t t0 = std::clock();
#endif

  // Get data about the structure to be optimised
  PotentialData & data = evaluator.getData();

  double h, h0, dH;
  const size_t numParticles = structure.getNumAtoms();

  // Position matrices
  arma::mat pos(3, numParticles), deltaPos(3, numParticles);
  structure.getAtomPositions(pos);
  // Forces, current are in data.myForces
  arma::mat f0(3, numParticles), deltaF(3, numParticles);
  arma::rowvec fSqNorm(arma::zeros(1, numParticles));
  arma::mat33 absResidualStress;

  double xg, gg;
  arma::vec3 f;

  data.forces.set_size(3, numParticles);
  data.forces.ones();
  deltaPos.zeros();

  // Initialisation of variables
  dH = std::numeric_limits< double>::max();
  h = 1.0;

  const double dNumAtoms = static_cast< double>(numParticles);

  bool converged = false;
  size_t numLastEvaluationsWithProblem = 0;

  OptimisationOutcome outcome = OptimisationOutcome::success();

  // Set the initial step size so get moving
  double stepsize = INITIAL_STEPSIZE;
  unsigned int iter;
  for(iter = 0; !converged && iter < *settings.maxIter; ++iter)
  {
    // Save the energy and forces from last time around
    h0 = h;
    f0 = data.forces;

    // Evaluate the potential
    data.reset();
    if(!evaluator.evalPotential().second)
    {
      // Couldn't evaluate potential for some reason.  Probably the unit cell
      // has collapsed and there are too many r12 vectors to evaluate.
      ++numLastEvaluationsWithProblem;
    }
    else
    {
      // That evaluation was fine, so reset counter
      numLastEvaluationsWithProblem = 0;
    }

    h = data.internalEnergy;

    deltaF = data.forces - f0;

    fSqNorm = arma::sum(data.forces % data.forces);
    // The % operator does the Schur product i.e.
    // element wise multiplication of two matrices
    // The accu function will do the sum of all elements
    xg = accu(deltaPos % deltaF);
    gg = accu(deltaF % deltaF);

    if(fabs(xg) > 0.0)
      stepsize = fabs(xg / gg);

    deltaPos = stepsize * data.forces;
    // Check if the stepsize needs to be capped
    if(capStepsize(structure, &deltaPos, NULL, settings, &stepsize))
    {
      // Recalculate the step
      if(*settings.optimisationType & OptimisationSettings::Optimise::ATOMS)
        deltaPos = stepsize * data.forces;
    }

    pos += deltaPos;

    // Tell the structure about the new positions
    structure.setAtomPositions(pos);

    dH = h - h0;
    converged = hasConverged(dH / dNumAtoms, fSqNorm.max(), 0.0, settings);
  }

  // Only a successful optimisation if it has converged
  // and the last potential evaluation had no problems
  if(outcome.isSuccess() && numLastEvaluationsWithProblem != 0)
    outcome = OptimisationOutcome::failure(
        OptimisationError::ERROR_EVALUATING_POTENTIAL,
        "Potential evaluation errors during optimisation");

  if(outcome.isSuccess() && !converged)
  {
    std::stringstream ss;
    ss << "Failed to converge after " << *settings.maxIter << " steps";
    outcome = OptimisationOutcome::failure(
        OptimisationError::FAILED_TO_CONVERGE, ss.str());
  }

  populateOptimistaionData(optimisationData, structure, data);
  optimisationData.numIters.reset(iter - 1);

#ifdef TPSD_GEOM_OPTIMISER_TIMING
  const clock_t t = std::clock() - t0;
  std::cout << "Optimisation took: " << static_cast<float>(t) / CLOCKS_PER_SEC << std::endl;
#endif

  return outcome;
}

OptimisationOutcome
TpsdGeomOptimiser::optimise(common::Structure & structure,
    common::UnitCell & unitCell, OptimisationData & optimisationData,
    IPotentialEvaluator & evaluator,
    const OptimisationSettings & settings) const
{
  SSLIB_ASSERT(settings.maxIter.is_initialized());
  SSLIB_ASSERT(settings.pressure.is_initialized());

#ifdef TPSD_GEOM_OPTIMISER_TIMING
  const clock_t t0 = std::clock();
#endif

#if TPSD_GEOM_OPTIMISER_DEBUG
  TpsdGeomOptimiserDebugger debugger;
#endif

  const size_t numParticles = structure.getNumAtoms();

  // Set up the external pressure
  const arma::mat33 pressureMtx = *settings.pressure;
  const double extPressure = arma::trace(pressureMtx) / 3.0;

  // Get data about the structure to be optimised
  PotentialData & data = evaluator.getData();

  // Stress and lattice matrices
  arma::mat33 s, s0, deltaS, deltaLatticeCar;
  // Position matrices
  arma::mat pos(3, numParticles), deltaPos(3, numParticles);
  structure.getAtomPositions(pos);
  // Forces, current are in data.force
  arma::mat f0(3, numParticles), deltaF(3, numParticles);
  arma::rowvec fSqNorm(arma::zeros(1, numParticles));
  arma::mat33 residualStress;

  double volume;
  double xg, gg;

  data.forces.set_size(3, numParticles);
  data.forces.ones();
  deltaPos.zeros();
  deltaLatticeCar.zeros();

  // Niggli reduce the unit cell as it makes cell optimisation faster
  unitCell.niggliReduce();
  arma::mat33 latticeCar = unitCell.getOrthoMtx();

  // Change in enthalpy between steps
  double dH = std::numeric_limits< double>::max();
  // Enthalpy = U + pV
  double h0, h = 1.0;
  s.ones();

  const double dNumAtoms = static_cast< double>(numParticles);

  bool converged = false;
  size_t numLastEvaluationsWithProblem = 0;

  OptimisationOutcome outcome = OptimisationOutcome::success();

  // Set the initial step size so get moving
  double step = INITIAL_STEPSIZE;
  unsigned int iter;
  for(iter = 0; !converged && iter < *settings.maxIter; ++iter)
  {
    h0 = h;
    f0 = data.forces;
    s0 = s;

    volume = unitCell.getVolume();

    // Evaluate the potential
    data.reset();
    if(!evaluator.evalPotential().second)
    {
      // Couldn't evaluate potential for some reason.  Probably the unit cell
      // has collapsed and there are too many r12 vectors to evaluate.
      ++numLastEvaluationsWithProblem;
    }
    else
    {
      // That evaluation was fine, so reset counter
      numLastEvaluationsWithProblem = 0;
    }

    s = data.stressMtx * latticeCar;
    // Calculate the enthalpy
    h = data.internalEnergy + extPressure * volume;

    deltaF = data.forces - f0;

    xg = gg = 0.0;
    if(*settings.optimisationType & OptimisationSettings::Optimise::ATOMS)
    {
      fSqNorm = arma::sum(data.forces % data.forces);
      // The accu function will do the sum of all elements
      // and the % operator does the Shure product i.e.
      // element wise multiplication of two matrices
      xg = accu(deltaPos % deltaF);
      gg = accu(deltaF % deltaF);
    }

    deltaS = s - s0;
    if(*settings.optimisationType & OptimisationSettings::Optimise::LATTICE)
    {
      xg += accu(deltaLatticeCar % deltaS);
      gg += accu(deltaS % deltaS);
    }

    if(std::fabs(xg) > 0.0)
      step = std::fabs(xg / gg);

    residualStress = data.stressMtx + pressureMtx;

    if(*settings.optimisationType & OptimisationSettings::Optimise::ATOMS)
      deltaPos = step * data.forces;
    if(*settings.optimisationType & OptimisationSettings::Optimise::LATTICE)
      deltaLatticeCar = -step * residualStress * latticeCar;

    // Check if the stepsize needs to be capped
    if(capStepsize(structure, &deltaPos, &deltaLatticeCar, settings, &step))
    {
      // Recalculate the step
      if(*settings.optimisationType & OptimisationSettings::Optimise::ATOMS)
        deltaPos = step * data.forces;
      if(*settings.optimisationType & OptimisationSettings::Optimise::LATTICE)
        deltaLatticeCar = -step * residualStress * latticeCar;
    }

    if(*settings.optimisationType & OptimisationSettings::Optimise::ATOMS)
      pos += deltaPos; // Move the particles on by a step

    // Fractionalise coordinates and wrap coordinates
    unitCell.cartsToFracInplace(pos);
    unitCell.wrapVecsFracInplace(pos);

    if(*settings.optimisationType & OptimisationSettings::Optimise::LATTICE)
    {
      // Move on cell vectors to relax stress
      latticeCar += deltaLatticeCar;
      if(!unitCell.setOrthoMtx(latticeCar))
      {
        // The unit cell matrix has become singular
        outcome = OptimisationOutcome::failure(
            OptimisationError::PROBLEM_WITH_STRUCTURE,
            "Lattice matrix has become singular.");
        break;
      }
    }

    // Finally re-orthogonalise the ion positions
    unitCell.fracsToCartInplace(pos);

    // Tell the structure about the new positions
    structure.setAtomPositions(pos);

    dH = h - h0;

    residualStress = arma::abs(residualStress);
    converged = hasConverged(dH / dNumAtoms, fSqNorm.max(),
        residualStress.max(), settings);

#if TPSD_GEOM_OPTIMISER_DEBUG
    debugger.postOptStepDebugHook(structure, i);
#endif

    if((iter + 1 % CHECK_CELL_EVERY_N_STEPS == 0) && !cellReasonable(unitCell))
    {
      outcome = OptimisationOutcome::failure(
          OptimisationError::PROBLEM_WITH_STRUCTURE,
          "Unit cell has collapsed.");
      break;
    }
  }

  // Wrap the particle positions so they stay in the central unit cell
  unitCell.wrapVecsInplace(pos);

  // Only a successful optimisation if it has converged
  // and the last potential evaluation had no problems
  if(outcome.isSuccess() && numLastEvaluationsWithProblem != 0)
    outcome = OptimisationOutcome::failure(
        OptimisationError::ERROR_EVALUATING_POTENTIAL,
        "Potential evaluation errors during optimisation");

  if(outcome.isSuccess() && !converged)
  {
    std::stringstream ss;
    ss << "Failed to converge after " << *settings.maxIter << " steps";
    outcome = OptimisationOutcome::failure(
        OptimisationError::FAILED_TO_CONVERGE, ss.str());
  }

#if TPSD_GEOM_OPTIMISER_DEBUG
  std::cout << "Optimised after " << i << " iterations." << std::endl;
#endif

  populateOptimistaionData(optimisationData, structure, data);
  optimisationData.numIters.reset(iter - 1);

#ifdef TPSD_GEOM_OPTIMISER_TIMING
  const clock_t t = std::clock() - t0;
  std::cout << "Optimisation took: " << static_cast<float>(t) / CLOCKS_PER_SEC << std::endl;
#endif

  return outcome;
}

bool
TpsdGeomOptimiser::cellReasonable(const spl::common::UnitCell & unitCell) const
{
  // Do a few checks to see if the cell has collapsed
  if(unitCell.getNormVolume() < CELL_MIN_NORM_VOLUME)
    return false;

  const double (&params)[6] = unitCell.getLatticeParams();

  if(params[3] + params[4] + params[5] > CELL_MAX_ANGLE_SUM)
    return false;

  return true;
}

void
TpsdGeomOptimiser::populateOptimistaionData(OptimisationData & optData,
    const common::Structure & structure, const PotentialData & potData) const
{
  const common::UnitCell * const unitCell = structure.getUnitCell();

  optData.internalEnergy.reset(potData.internalEnergy);
  const double pressure = -::arma::trace(potData.stressMtx) / 3.0;
  optData.pressure.reset(pressure);
  if(unitCell)
  {
    optData.enthalpy.reset(
        potData.internalEnergy + *optData.pressure * unitCell->getVolume());
  }
  optData.ionicForces.reset(potData.forces);
  optData.stressMtx.reset(potData.stressMtx);
}

bool
TpsdGeomOptimiser::hasConverged(const double deltaEnergyPerIon,
    const double maxForceSq, const double maxStress,
    const OptimisationSettings & options) const
{
  bool converged = true;

  // TODO: Add lattice stress
  //if(*options.optimisationType & OptimisationSettings::Optimise::LATTICE)

  if(options.stressTol && maxStress > *options.stressTol)
    return false;

  if(*options.optimisationType & OptimisationSettings::Optimise::ATOMS)
    converged &= std::abs(deltaEnergyPerIon) < *options.energyTol
        && maxForceSq < *options.forceTol * *options.forceTol;

  return converged;
}

bool
TpsdGeomOptimiser::capStepsize(const common::Structure & structure,
    const arma::mat * const deltaPos, const arma::mat * const deltaLatticeCar,
    const OptimisationSettings & settings, double * const stepsize) const
{
  bool cappedStep = false;
  const double originalStepsize = *stepsize;
  boost::optional< double> cellVolume;
  if(structure.getUnitCell())
    cellVolume.reset(structure.getUnitCell()->getVolume());

  if(deltaPos
      && (*settings.optimisationType & OptimisationSettings::Optimise::ATOMS))
  {
    const int numAtoms = structure.getNumAtoms();
    double meanDist = 0.0;
    if(cellVolume)
      meanDist = std::pow(*cellVolume / static_cast< double>(numAtoms),
          1.0 / 3.0);
    else
    {
      // Have no unit cell so have to get a characteristic distance by evaluating
      // atom pair distances
      const common::DistanceCalculator & distCalc =
          structure.getDistanceCalculator();
      meanDist = 0.0;
      for(int i = 0; i < numAtoms - 1; ++i)
      {
        const common::Atom & atom1 = structure.getAtom(i);
        for(int j = i; j < numAtoms; ++j)
          meanDist = distCalc.getDistMinImg(atom1, structure.getAtom(j));
      }
      meanDist /= static_cast< double>(numAtoms * numAtoms / 2);
    }

    const arma::mat absDeltaPos = arma::abs(*deltaPos);
    const double maxDeltaPos = absDeltaPos.max();
    if(maxDeltaPos > DEFAULT_MAX_DELTA_POS_FACTOR * meanDist)
    {
      *stepsize = originalStepsize * DEFAULT_MAX_DELTA_LATTICE_FACTOR * meanDist
          / maxDeltaPos;
      cappedStep = true;
    }
  }
  if(deltaLatticeCar
      && (*settings.optimisationType & OptimisationSettings::Optimise::LATTICE))
  {
    const arma::mat absDeltaLatticeCar = arma::abs(*deltaLatticeCar);
    const double lengthUnit = std::pow(*cellVolume, 1.0 / 3.0);
    const double maxDeltaLatticeCar = absDeltaLatticeCar.max();

    if(maxDeltaLatticeCar > DEFAULT_MAX_DELTA_LATTICE_FACTOR * lengthUnit)
    {
      const double cappedStepsize = originalStepsize
          * DEFAULT_MAX_DELTA_LATTICE_FACTOR * lengthUnit / maxDeltaLatticeCar;
      *stepsize = std::min(*stepsize, cappedStepsize);
      cappedStep = true;
    }
  }
  return cappedStep;
}

}
}

