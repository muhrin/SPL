/*
 * HasProperties.h
 *
 *  Created on: Sep 29, 2013
 *      Author: Martin Uhrin
 */

#ifndef SPL__UTILITY__HAS_PROPERTIES_H_
#define SPL__UTILITY__HAS_PROPERTIES_H_

// INCLUDES ///////////////////////////////////////////////
#include "spl/SSLib.h"

#include "spl/utility/HeterogeneousMap.h"

namespace spl {
namespace utility {

class HasProperties
{
public:
  HasProperties()
  {
  }
  HasProperties(const HasProperties & toCopy) :
      myTypedProperties(toCopy.myTypedProperties)
  {
  }

  HasProperties &
  operator =(const HasProperties & rhs)
  {
    myTypedProperties = rhs.myTypedProperties;
    return *this;
  }

  HeterogeneousMap &
  properties()
  {
    return myTypedProperties;
  }

  const HeterogeneousMap &
  properties() const
  {
    return myTypedProperties;
  }

private:
  HeterogeneousMap myTypedProperties;
};

}
}

#endif /* SPL__UTILITY__HAS_PROPERTIES_H_ */
