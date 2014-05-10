/*
 * Histogram.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "spl/analysis/Histogram.h"

#include <cmath>
#include <iostream>

#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"

namespace spl {
namespace analysis {

Histogram::Histogram(const double binWidth):
myBinWidth(binWidth)
{}

void Histogram::insert(const double value)
{
  const size_t idx = binIndex(value);
  ensureEnoughBins(idx);
  ++myBins[idx];
}

void Histogram::clear()
{
  myBins.clear();
}

size_t Histogram::numBins() const
{
  return myBins.size();
}

unsigned int Histogram::getFrequency(const size_t bin) const
{
  SSLIB_ASSERT(bin < myBins.size());

  return myBins[bin];
}

Histogram::iterator Histogram::begin()
{
  return myBins.begin();
}

Histogram::const_iterator Histogram::begin() const
{
  return myBins.begin();
}

Histogram::iterator Histogram::end()
{
  return myBins.end();
}

Histogram::const_iterator Histogram::end() const
{
  return myBins.end();
}

void Histogram::print(::std::ostream & os) const
{
  const int fullest = getFullestBin();
  if(fullest == -1)
    return;

  const size_t numBins = myBins.size();
  unsigned int largestValue = myBins[fullest];
  for(unsigned int row = largestValue; row > 0; --row)
  {
    ::std::cout << "|";
    for(size_t col = 0; col < numBins; ++col)
    {
      if(myBins[col] >= row)
        ::std::cout << "*";
      else
        ::std::cout << " ";
    }
    ::std::cout << "\n";
  }
  // Draw the x-axis
  ::std::cout << "O";
  for(unsigned int col = 0; col < numBins; ++col)
    ::std::cout << "-";
  ::std::cout << ::std::endl;

  ::std::cout << "0 to " << myBinWidth * static_cast<double>(numBins) <<
    numBins << " bin width: " << myBinWidth << ::std::endl;
    
}

void Histogram::ensureEnoughBins(const unsigned int binIndex)
{
  if(myBins.size() <= binIndex)
    myBins.resize(binIndex + 1);
}

size_t Histogram::binIndex(const double value)
{
  return static_cast<size_t>(::std::floor(value / myBinWidth));
}

int Histogram::getFullestBin() const
{
  if(myBins.empty())
    return -1;
  
  int fullest = 0;
  // Go from the 2nd bin onwards
  for(size_t i = 1; i < myBins.size(); ++i)
  {
    if(myBins[i] > myBins[fullest])
      fullest = i;
  }
  return fullest;
}


}
}

std::ostream & operator<<(std::ostream & os, const spl::analysis::Histogram & hist)
{
  hist.print(os);
  return os;
}
