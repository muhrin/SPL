/*
 * OptimisationSettings.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef OPTIMISATION_SETTINGS_H
#define OPTIMISATION_SETTINGS_H

// INCLUDES /////////////////////////////////////////////
#include <vector>

#include <boost/ptr_container/ptr_list.hpp>

#include <armadillo>

#include "spl/OptionalTypes.h"
#include "spl/common/Types.h"

// DEFINES //////////////////////////////////////////////

namespace spl {
namespace potential {

// FORWARD DECLARATIONS ////////////////////////////////////

struct OptimisationSettings
{
  struct Optimise
  {
    enum Value
    {
      ATOMS = 0x01, // 001
      LATTICE = 0x02, // 010
      ATOMS_AND_LATTICE = 0x03 // 011
    };
  };

  OptionalUInt maxIter;
  OptionalDouble energyTol;
  OptionalDouble forceTol;
  OptionalDouble stressTol;
  OptionalArmaMat33 pressure;
  boost::optional< Optimise::Value> optimisationType;
};

}
}

#endif /* OPTIMISATION_SETTINGS_H */
