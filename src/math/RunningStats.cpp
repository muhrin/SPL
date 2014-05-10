/*
 * RunningStats.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "spl/math/RunningStats.h"

#include <limits>
#include <cmath>

namespace spl {
namespace math {

RunningStats::RunningStats():
myNum(0),
mySum(0.0),
mySqSum(0.0),
myMin(::std::numeric_limits<double>::max()),
myMax(-::std::numeric_limits<double>::max())
{}

void RunningStats::insert(const double x)
{
  ++myNum;
  mySum += x;
  mySqSum += x * x;
  myMin = ::std::min(myMin, x);
  myMax = ::std::max(myMax, x);
}

unsigned int RunningStats::num() const
{
  return myNum;
}

double RunningStats::min() const
{
  return myMin;
}

double RunningStats::max() const
{
  return myMax;
}

double RunningStats::mean() const
{
  return mySum / static_cast<double>(myNum);
}

double RunningStats::sum() const
{
  return mySum;
}

double RunningStats::sqSum() const
{
  return mySqSum;
}

double RunningStats::rms() const
{
  return ::std::sqrt(mySqSum / static_cast<double>(myNum));
}

}
}
