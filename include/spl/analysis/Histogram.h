/*
 * Histogram.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <algorithm>
#include <limits>
#include <ostream>
#include <vector>

#include "spl/math/RunningStats.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

class Histogram
{
  typedef ::std::vector<unsigned int> Bins;
public:
  typedef Bins::iterator iterator;
  typedef Bins::const_iterator const_iterator;

  template <class InputIterator>
  static double estimateBinWidth(
    InputIterator first,
    const InputIterator last,
    const double targetMeanFrequency = 2.0,
    const size_t maxBins = ::std::numeric_limits<size_t>::max()
  );

  Histogram(const double binWidth);

  void insert(const double value);
  template <class InputIterator>
  void insert(InputIterator first, const InputIterator last);
  void clear();
  size_t numBins() const;
  unsigned int getFrequency(const size_t bin) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  void print(::std::ostream & os) const;

private:

  void ensureEnoughBins(const unsigned int binIndex);
  size_t binIndex(const double value);
  int getFullestBin() const;


  const double myBinWidth;
  Bins myBins;

};

template <class InputIterator>
double Histogram::estimateBinWidth(
  InputIterator first,
  const InputIterator last,
  const double targetMeanFrequency,
  const size_t maxBins
)
{
  if(first == last)
    return 0.0;

  ::std::vector<double> values(first, last);
  ::std::sort(values.begin(), values.end());

  ::spl::math::RunningStats diffStats;
  diffStats.insert(values[0]); // Put in the first point to account for 0->values[i]
  for(size_t i = 0; i < values.size() - 1; ++i)
  {
    diffStats.insert(values[i + 1] - values[i]);
  }
  const double maxBinsWidth = values.back() / static_cast<double>(maxBins);
  return ::std::max(diffStats.mean() * targetMeanFrequency, maxBinsWidth);
}

template <class InputIterator>
void Histogram::insert(InputIterator first, const InputIterator last)
{
  for(;first != last; ++first)
    insert(*first);
}

}
}

std::ostream & operator<<(std::ostream & os, const spl::analysis::Histogram & hist);

#endif /* HISTOGRAM_H */
