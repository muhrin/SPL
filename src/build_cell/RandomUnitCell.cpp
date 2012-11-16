/*
 * RandomUnitCell.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////////
#include "build_cell/RandomUnitCell.h"

#include "SSLibAssert.h"
#include "common/UnitCell.h"
#include "common/Utils.h"


namespace sstbx {
namespace build_cell {

const double RandomUnitCell::DEFAULT_MIN_ANGLE = 30.0;
const double RandomUnitCell::DEFAULT_MAX_ANGLE = 140.0;
const double RandomUnitCell::DEFAULT_MIN_LENGTH = 0.25;
const double RandomUnitCell::DEFAULT_MAX_LENGTH = 2.0;
const double RandomUnitCell::DEFAULT_TARGET_VOLUME = 50.0;
const double RandomUnitCell::DEFAULT_VOLUME_DELTA = 0.25;
const double RandomUnitCell::DEFAULT_MAX_LENGTH_RATIO = 6.0;

RandomUnitCell::ParamValue RandomUnitCell::getMin(const size_t param) const
{
  SSLIB_ASSERT(param < 6);

  ParamValue returnVal;
  if(myParameters[param].first)
  {
    returnVal.first = *myParameters[param].first;
    returnVal.second = true;
  }
  else
  {
    if(param < 3)
    {
      returnVal.first = DEFAULT_MIN_LENGTH;
    }
    else
    {
      returnVal.first = DEFAULT_MIN_ANGLE;
    }
    returnVal.second = false;
  }

  return returnVal;
}

RandomUnitCell::ParamValue RandomUnitCell::getMax(const size_t param) const
{
  SSLIB_ASSERT(param < 6);

  ParamValue returnVal;
  if(myParameters[param].second)
  {
    returnVal.first = *myParameters[param].second;
    returnVal.second = true;
  }
  else
  {
    if(param < 3)
    {
      returnVal.first = DEFAULT_MAX_LENGTH;
    }
    else
    {
      returnVal.first = DEFAULT_MAX_ANGLE;
    }
    returnVal.second = false;
  }

  return returnVal;
}

void RandomUnitCell::setMin(const size_t param, const OptionalDouble min)
{
  SSLIB_ASSERT(param < 6);

  myParameters[param].first = min;
}

void RandomUnitCell::setMax(const size_t param, const OptionalDouble max)
{
  SSLIB_ASSERT(param < 6);

  myParameters[param].second = max;
}

void RandomUnitCell::setMinLengths(const OptionalDouble length)
{
  using namespace utility::cell_params_enum;

  for(size_t i = A; i <= C; ++i)
  {
    myParameters[i].first = length;
  }
}

void RandomUnitCell::setMaxLengths(const OptionalDouble length)
{
  using namespace utility::cell_params_enum;

  for(size_t i = A; i <= C; ++i)
  {
    myParameters[i].second = length;
  }
}

void RandomUnitCell::setMinAngles(const OptionalDouble angle)
{
  using namespace utility::cell_params_enum;

  for(size_t i = ALPHA; i <= GAMMA; ++i)
  {
    myParameters[i].first = angle;
  }
}

void RandomUnitCell::setMaxAngles(const OptionalDouble angle)
{
  using namespace utility::cell_params_enum;
  
  for(size_t i = ALPHA; i <= GAMMA; ++i)
  {
    myParameters[i].second = angle;
  }
}

void RandomUnitCell::setTargetVolume(const OptionalDouble volume)
{
  myTargetVolume = volume;
}

void RandomUnitCell::setVolumeDelta(const OptionalDouble delta)
{
  myVolumeDelta = delta;
}

void RandomUnitCell::setMaxLengthRatio(const OptionalDouble maxLengthRatio)
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

RandomUnitCell::ParamValue RandomUnitCell::getMaxLengthRatio() const
{
  if(myMaxLengthRatio)
    return ParamValue(*myMaxLengthRatio, true);
  else
    return ParamValue(DEFAULT_MAX_LENGTH_RATIO, false);
}

common::UnitCellPtr RandomUnitCell::generateCell(const RandomUnitCell::OptionalStructureInfo structureInfo) const
{
  using namespace utility::cell_params_enum;

  double params[6];

  size_t i;
  generateLengths(params);

  bool anglesValid = false;
  for(size_t iters = 0; iters < 1000 && !anglesValid; ++iters)
  {
    for(i = ALPHA; i <= GAMMA; ++i)
    {
      params[i] = generateParameter(i);
    }
    anglesValid = areParametersValid(params);
  }

  // If the angles still aren't valid then use the
  // minimum values for all of them
  if(!anglesValid)
  {
    for(i = ALPHA; i <= GAMMA; ++i)
    {
      params[i] = getMin(i).first;
    }
  }

  // TODO: Deal with possible runtime exception as a result of
  // ortho matrix being singular
  common::UnitCellPtr cell(new common::UnitCell(params));

  if(!myTargetVolume && structureInfo)
  {
    cell->setVolume(2.0 * structureInfo->atomsVolume); 
  }
  else
  {
    cell->setVolume(generateVolume());
  }

  return cell;
}

double RandomUnitCell::generateParameter(const size_t param) const
{
  SSLIB_ASSERT(param < 6);

  if(isLength(param))
  {
    const double min = myParameters[param].first ? *myParameters[param].first : DEFAULT_MIN_LENGTH;
    const double max = myParameters[param].second ? *myParameters[param].second : DEFAULT_MAX_LENGTH;

    return common::randDouble(min, max);
  }
  else
  {
    const double min = myParameters[param].first ? *myParameters[param].first : DEFAULT_MIN_ANGLE;
    const double max = myParameters[param].second ? *myParameters[param].second : DEFAULT_MAX_ANGLE;

    return common::randDouble(min, max);    
  }
}

void RandomUnitCell::generateLengths(double (&params)[6]) const
{
  using namespace utility::cell_params_enum;

  const size_t maxIters = 10;

  // Generate the lengths
  for(size_t i = A; i <= C; ++i)
  {
    params[i] = generateParameter(i);
  }

  MinMaxIndex minMax;

  const bool overrideLengthRatio = myMaxLengthRatio;
  const double maxRatio = overrideLengthRatio ? *myMaxLengthRatio : DEFAULT_MAX_LENGTH_RATIO;
  double minLength, maxLength, ratio, cFactor;
  bool canMoveMin, canMoveMax, madeStep = true;
  size_t i;
  for(i = 0; i < maxIters && madeStep; ++i)
  {
    madeStep = false;
    minMax = getMinMaxLengths(params);

    minLength = params[minMax.first];
    maxLength = params[minMax.second];

    ratio = maxLength / minLength;

    if(ratio > maxRatio)
    {
      // Can move if the user hasn't set the parameter or if we are still within
      // the user set range
      canMoveMin = !myParameters[minMax.first].first || minLength > *myParameters[minMax.first].first;
      canMoveMax = !myParameters[minMax.second].second || maxLength < *myParameters[minMax.second].second;

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
      if(minLength != params[minMax.first] || maxLength != params[minMax.second])
      {
        params[minMax.first] = minLength;
        params[minMax.second] = maxLength;
        madeStep = true;
      }
    }
  }

  SSLIB_ASSERT(i < maxIters);
}

double RandomUnitCell::generateVolume(const double overrideVolume) const
{
  double target = overrideVolume;
  if(target == 0.0)
  {
    target = myTargetVolume ? *myTargetVolume : DEFAULT_TARGET_VOLUME;
  }

  const double delta = myVolumeDelta ? *myVolumeDelta : DEFAULT_VOLUME_DELTA;

  return common::randDouble(target * (1.0 - delta), target * (1 + delta));
}

RandomUnitCell::MinMaxIndex RandomUnitCell::getMinMaxLengths(const double (&params)[6]) const
{
  using namespace utility::cell_params_enum;

  MinMaxIndex minMax;
  if(params[A] > params[B])
  {
    minMax.first  = B;
    minMax.second = A;
  }
  else
  {
    minMax.first  = A;
    minMax.second = B;
  }

  if(params[minMax.first] > params[C])
    minMax.first = C;
  else if(params[minMax.second] < params[C])
    minMax.second = C;

  return minMax;
}

bool RandomUnitCell::areParametersValid(const double (&params)[6]) const
{
  using namespace utility::cell_params_enum;

  const double anglesSum = params[ALPHA] + params[BETA] + params[GAMMA];

  if(anglesSum >= 360.0) return false;
  if(abs(params[ALPHA]-params[BETA]) > params[GAMMA]) return false;
  if(abs(params[BETA]-params[GAMMA]) > params[ALPHA]) return false;
  if(abs(params[GAMMA]-params[ALPHA]) > params[BETA]) return false;

  return true;
}

}
}
