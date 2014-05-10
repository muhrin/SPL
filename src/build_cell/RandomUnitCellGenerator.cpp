/*
 * RandomUnitCellGenerator.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////////
#include "spl/build_cell/RandomUnitCellGenerator.h"

#include "spl/SSLibAssert.h"
#include "spl/build_cell/GenerationOutcome.h"
#include "spl/build_cell/StructureContents.h"
#include "spl/common/UnitCell.h"
#include "spl/math/Random.h"
#include "spl/utility/StableComparison.h"

namespace spl {
namespace build_cell {

namespace comp = utility::stable;

const double RandomUnitCellGenerator::DEFAULT_MIN_ANGLE = 35.0;
const double RandomUnitCellGenerator::DEFAULT_MAX_ANGLE = 135.0;
const double RandomUnitCellGenerator::DEFAULT_MIN_LENGTH = 0.25;
const double RandomUnitCellGenerator::DEFAULT_MAX_LENGTH = 2.0;
const double RandomUnitCellGenerator::DEFAULT_TARGET_VOLUME = 50.0;
const double RandomUnitCellGenerator::DEFAULT_VOLUME_DELTA = 0.25;
const double RandomUnitCellGenerator::DEFAULT_MAX_LENGTH_RATIO = 4.0;
const double RandomUnitCellGenerator::DEFAULT_BULK_CONTENTS_MULTIPLIER = 2.0;
const double RandomUnitCellGenerator::DEFAULT_CLUSTER_CONTENTS_MULTIPLIER = 10.0;

RandomUnitCellGenerator::ParameterValueAndSpecific
RandomUnitCellGenerator::getMin(const size_t param) const
{
  SSLIB_ASSERT(param < 6);

  ParameterValueAndSpecific returnVal;
  if(myParameters[param].first)
  {
    returnVal.first = *myParameters[param].first;
    returnVal.second = true;
  }
  else
  {
    if(isLength(param))
      returnVal.first = myMinLength ? *myMinLength : DEFAULT_MIN_LENGTH;
    else
      returnVal.first = myMinAngle ? *myMinAngle : DEFAULT_MIN_ANGLE;
    returnVal.second = false;
  }

  return returnVal;
}

RandomUnitCellGenerator::ParameterValueAndSpecific
RandomUnitCellGenerator::getMax(const size_t param) const
{
  SSLIB_ASSERT(param < 6);

  ParameterValueAndSpecific returnVal;
  if(myParameters[param].second)
  {
    returnVal.first = *myParameters[param].second;
    returnVal.second = true;
  }
  else
  {
    if(isLength(param))
      returnVal.first = myMaxLength ? *myMaxLength : DEFAULT_MAX_LENGTH;
    else
      returnVal.first = myMaxAngle ? *myMaxAngle : DEFAULT_MAX_ANGLE;
    returnVal.second = false;
  }

  return returnVal;
}

void
RandomUnitCellGenerator::setMin(const size_t param, const OptionalDouble min)
{
  SSLIB_ASSERT(param < 6);

  myParameters[param].first = min;
}

void
RandomUnitCellGenerator::setMax(const size_t param, const OptionalDouble max)
{
  SSLIB_ASSERT(param < 6);

  myParameters[param].second = max;
}

void
RandomUnitCellGenerator::setMinLengths(const OptionalDouble length)
{
  myMinLength = length;
}

void
RandomUnitCellGenerator::setMaxLengths(const OptionalDouble length)
{
  myMaxLength = length;
}

void
RandomUnitCellGenerator::setMinAngles(const OptionalDouble angle)
{
  myMinAngle = angle;
}

void
RandomUnitCellGenerator::setMaxAngles(const OptionalDouble angle)
{
  myMaxAngle = angle;
}

void
RandomUnitCellGenerator::setTargetVolume(const OptionalDouble volume)
{
  myTargetVolume = volume;
}

void
RandomUnitCellGenerator::setVolumeDelta(const OptionalDouble delta)
{
  myVolumeDelta = delta;
}

void
RandomUnitCellGenerator::setContentsMultiplier(
    const OptionalDouble contentsMultiplier)
{
  myContentsMultiplier = contentsMultiplier;
}

void
RandomUnitCellGenerator::setMaxLengthRatio(const OptionalDouble maxLengthRatio)
{
  myMaxLengthRatio = maxLengthRatio;
  if(myMaxLengthRatio)
  {
    const double ratio = *myMaxLengthRatio;
    // It if is less than one than assume the user specified a reciprocal value
    if(ratio < 1.0)
      myMaxLengthRatio = 1 / ratio;
  }
}

RandomUnitCellGenerator::ParameterValueAndSpecific
RandomUnitCellGenerator::getMaxLengthRatio() const
{
  if(myMaxLengthRatio)
    return ParameterValueAndSpecific(*myMaxLengthRatio, true);
  else
    return ParameterValueAndSpecific(DEFAULT_MAX_LENGTH_RATIO, false);
}

GenerationOutcome
RandomUnitCellGenerator::generateCell(common::UnitCellPtr & cellOut,
    const bool structureIsCluster) const
{
  GenerationOutcome outcome = generateLatticeParameters(cellOut);
  if(!outcome.isSuccess())
    return outcome;

  const VolAndDelta volAndDelta = generateVolumeParams(cellOut->getVolume(),
      structureIsCluster);
  cellOut->setVolume(generateVolume(volAndDelta));

  return GenerationOutcome::success();
}

GenerationOutcome
RandomUnitCellGenerator::generateCell(common::UnitCellPtr & cellOut,
    const StructureContents & structureContents,
    const bool structureIsCluster) const
{
  GenerationOutcome outcome = generateLatticeParameters(cellOut);
  if(!outcome.isSuccess())
    return outcome;

  const VolAndDelta volAndDelta = generateVolumeParams(cellOut->getVolume(),
      structureIsCluster, &structureContents);
  cellOut->setVolume(generateVolume(volAndDelta));

  return outcome;
}

IUnitCellGeneratorPtr
RandomUnitCellGenerator::clone() const
{
  return IUnitCellGeneratorPtr(new RandomUnitCellGenerator(*this));
}

double
RandomUnitCellGenerator::generateParameter(const size_t param) const
{
  SSLIB_ASSERT(param < 6);
  return math::randu(getMin(param).first, getMax(param).first);
}

GenerationOutcome
RandomUnitCellGenerator::generateLatticeParameters(
    common::UnitCellPtr & cellOut) const
{
  using namespace utility::cell_params_enum;

  double params[6];
  size_t i;
  generateLengths(params);

  bool anglesValid = false;
  for(size_t iters = 0; iters < 1000 && !anglesValid; ++iters)
  {
    for(i = ALPHA; i <= GAMMA; ++i)
      params[i] = generateParameter(i);

    anglesValid = areParametersValid(params);
  }

  // If the angles still aren't valid then use the
  // minimum values for all of them
  if(!anglesValid)
  {
    for(i = ALPHA; i <= GAMMA; ++i)
      params[i] = getMin(i).first;
  }

  try
  {
    cellOut.reset(new common::UnitCell(params));
  }
  catch(const ::std::runtime_error & /*e*/)
  {
    return GenerationOutcome::failure(
        "Cell parameters caused singular orthogonalisation matrix.");
  }
  return GenerationOutcome::success();
}

void
RandomUnitCellGenerator::generateLengths(double (&params)[6]) const
{
  using namespace utility::cell_params_enum;

  const size_t maxIters = 10;

  // Generate the lengths
  for(size_t i = A; i <= C; ++i)
  {
    params[i] = generateParameter(i);
  }

  OrderedPairIndex minMax;

  const bool overrideLengthRatio = myMaxLengthRatio;
  const double maxRatio =
      overrideLengthRatio ? *myMaxLengthRatio : DEFAULT_MAX_LENGTH_RATIO;
  double minLength, maxLength, ratio, cFactor;
  bool canMoveMin, canMoveMax, madeStep = true;
  size_t i;
  for(i = 0; i < maxIters && madeStep; ++i)
  {
    madeStep = false;
    minMax = getOrderedPairLengths(params);

    minLength = params[minMax.first];
    maxLength = params[minMax.second];

    ratio = maxLength / minLength;

    if(ratio > maxRatio)
    {
      // Can move if the user hasn't set the parameter or if we are still within
      // the user set range
      canMoveMin = !myParameters[minMax.first].first
          || comp::lt(minLength, *myParameters[minMax.first].second);
      canMoveMax = !myParameters[minMax.second].second
          || comp::gt(maxLength, *myParameters[minMax.second].first);

      // Three possibilities for movement
      if(canMoveMin && canMoveMax) // move both
      {
        cFactor = (ratio - maxRatio) / (ratio + maxRatio);
        minLength *= (1 + cFactor);
        maxLength *= (1 - cFactor);

        // Now constrain the lengths to not exceed user set min/maxf
        if(myParameters[minMax.first].second)
          minLength = ::std::min(minLength, *myParameters[minMax.first].second);
        if(myParameters[minMax.second].first)
          maxLength = ::std::max(maxLength, *myParameters[minMax.second].first);
      }
      else if(canMoveMin) // move minimum
      {
        minLength *= ratio / maxRatio;
        if(myParameters[minMax.first].second)
          minLength = ::std::min(minLength, *myParameters[minMax.first].second);
      }
      else if(canMoveMax) // move maximum
      {
        maxLength *= maxRatio / ratio;
        if(myParameters[minMax.second].first)
          maxLength = ::std::max(maxLength, *myParameters[minMax.second].first);
      }

      // Have we made any changes?
      if(minLength != params[minMax.first]
          || maxLength != params[minMax.second])
      {
        params[minMax.first] = minLength;
        params[minMax.second] = maxLength;
        madeStep = true;
      }
    }
  }
}

double
RandomUnitCellGenerator::generateVolume(const VolAndDelta & volAndDelta) const
{
  return math::randu(volAndDelta.first * (1.0 - volAndDelta.second),
      volAndDelta.first * (1.0 + volAndDelta.second));
}

RandomUnitCellGenerator::OrderedPairIndex
RandomUnitCellGenerator::getOrderedPairLengths(const double (&params)[6]) const
{
  using namespace utility::cell_params_enum;

  OrderedPairIndex minMax;
  if(params[A] > params[B])
  {
    minMax.first = B;
    minMax.second = A;
  }
  else
  {
    minMax.first = A;
    minMax.second = B;
  }

  if(params[minMax.first] > params[C])
    minMax.first = C;
  else if(params[minMax.second] < params[C])
    minMax.second = C;

  return minMax;
}

bool
RandomUnitCellGenerator::areParametersValid(const double (&params)[6]) const
{
  using ::std::abs;
  using namespace utility::cell_params_enum;

  const double anglesSum = params[ALPHA] + params[BETA] + params[GAMMA];

  if(anglesSum >= 360.0)
    return false;
  if(abs(params[ALPHA] - params[BETA]) > params[GAMMA])
    return false;
  if(abs(params[BETA] - params[GAMMA]) > params[ALPHA])
    return false;
  if(abs(params[GAMMA] - params[ALPHA]) > params[BETA])
    return false;

  return true;
}

bool
RandomUnitCellGenerator::cellFullySpecified() const
{
  using namespace utility::cell_params_enum;
  bool fullySpecified = true;
  for(size_t i = A; i <= GAMMA; ++i)
  {
    fullySpecified &= myParameters[i].first && myParameters[i].second;
  }
  return fullySpecified;
}

::std::pair< double, double>
RandomUnitCellGenerator::generateVolumeParams(const double currentVolume,
    const bool isCluster,
    const StructureContents * const structureContents) const
{
  VolAndDelta volAndDelta;

  if(myTargetVolume)
    volAndDelta.first = *myTargetVolume;
  else if(cellFullySpecified())
    volAndDelta.first = currentVolume;
  else if(structureContents && structureContents->getVolume() != 0.0)
  {
    double multiplier;
    if(myContentsMultiplier)
      multiplier = *myContentsMultiplier;
    else if(isCluster)
      multiplier = DEFAULT_CLUSTER_CONTENTS_MULTIPLIER;
    else
      multiplier = DEFAULT_BULK_CONTENTS_MULTIPLIER;

    volAndDelta.first = multiplier * structureContents->getVolume();
  }
  else
    volAndDelta.first = currentVolume;

  volAndDelta.second = myVolumeDelta ? *myVolumeDelta : DEFAULT_VOLUME_DELTA;

  return volAndDelta;
}

}
}
