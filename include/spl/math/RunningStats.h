/*
 * RunningStats.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef RUNNING_STATS_H
#define RUNNING_STATS_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <boost/optional.hpp>

// DEFINITION ///////////////////////

namespace spl {
namespace math {

template<typename T>
class GenericRunningStats
{
public:
  GenericRunningStats();

  void insert(const T & x);

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last);

  int num() const;
  bool empty() const;

  const T & min() const;
  const T & max() const;

  T mean() const;
  const T & sum() const;

  void clear();

private:
  int myNum;
  T mySum;
  T myMin;
  T myMax;
};

class RunningStats
{
public:
  RunningStats();

  void insert(const double x);

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last);

  unsigned int num() const;

  double min() const;
  double max() const;

  double mean() const;
  double sum() const;
  double sqSum() const;
  double rms() const;

private:
  unsigned int myNum;
  double mySum;
  double mySqSum;
  double myMin;
  double myMax;
};

template <class InputIterator>
void RunningStats::insert(InputIterator first, InputIterator last)
{
  for(InputIterator it = first; it != last; ++it)
    insert(*it);
}

template< typename T>
GenericRunningStats<T>::GenericRunningStats()
{
  clear();
}

template< typename T>
void
GenericRunningStats<T>::insert(const T & x)
{
  ++myNum;
  if(!empty())
  {
    mySum += x;
    myMin = ::std::min(myMin, x);
    myMax = ::std::max(myMax, x);
  }
  else
    mySum = myMin = myMax = x;
}

template <typename T>
template <class InputIterator>
void GenericRunningStats<T>::insert(InputIterator first, InputIterator last)
{
  for(InputIterator it = first; it != last; ++it)
    insert(*it);
}

template< typename T>
int
GenericRunningStats<T>::num() const
{
  return myNum;
}

template< typename T>
bool
GenericRunningStats<T>::empty() const
{
  return num() == 0;
}

template< typename T>
const T &
GenericRunningStats<T>::min() const
{
  return myMin;
}

template< typename T>
const T &
GenericRunningStats<T>::max() const
{
  return myMax;
}

template< typename T>
T
GenericRunningStats<T>::mean() const
{
  return mySum / myNum;
}

template< typename T>
const T &
GenericRunningStats<T>::sum() const
{
  return mySum;
}

template< typename T>
void
GenericRunningStats<T>::clear()
{
  myNum = 0;
  mySum = T();
  myMin = T();
  myMax = T();
}


}
}

#endif /* RUNNING_STATS_H */
