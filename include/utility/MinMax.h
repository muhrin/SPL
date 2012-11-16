/*
 * MinMax.h
 *
 * Tuple that represents minimum and maximum.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MIN_MAX_H
#define MIN_MAX_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"

#include <stdexcept>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace utility {

template <typename T>
class MinMax
{
public:

  typedef T ValueType;

  static MinMax makeMinMax(T value1, T value2)
  {
    return value1 < value2 ? MinMax(value1, value2) : MinMax(value2, value1)
  }

  MinMax(T & min, T & max):
    myMin(min),
    myMax(max)
  {
    ensureValid(myMin, myMax);
  }

  T & min() const { return myMin; }

  void setMin(const T & newMin)
  {
    ensureValid(newMin, myMax);
    myMin = newMin;
  }

  T & max() const { return max; }

  void setMax(const T & newMax)
  {
    ensureValid(myMin, newMax);
    myMax = newMax;
  }

private:

  inline void ensureValid(const T & min, const T & max)
  {
    if(min > max)
      throw ::std::invalid_argument("min: " + min + " max:" + max ". Require that min <= max.");
  }

  T myMin;
  T myMax;

};


// IMPLEMENTATION /////////////////////////////////////////////

}
}

#endif /* MIN_MAX_H */
