/*
 * GeomOptimiser.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef GEOM_OPTIMISER_H
#define GEOM_OPTIMISER_H

// INCLUDES /////////////////////////////////////////////

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <armadillo>

#include "spl/OptionalTypes.h"
#include "spl/common/Structure.h"
#include "spl/common/Types.h"
#include "spl/utility/Outcome.h"

// DEFINES //////////////////////////////////////////////

namespace spl {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
}
namespace potential {
class IPotential;
struct OptimisationSettings;

struct OptimisationError
{
  enum Value
  {
    FAILED_TO_CONVERGE,
    PROBLEM_WITH_STRUCTURE,
    ERROR_EVALUATING_POTENTIAL,
    INTERNAL_ERROR
  };
};

typedef utility::OutcomeWithErrorCode< OptimisationError::Value> OptimisationOutcome;

struct OptimisationData
{
  OptionalDouble internalEnergy;
  OptionalDouble enthalpy;
  OptionalDouble pressure;
  OptionalArmaMat33 pressureMtx;
  OptionalArmaMat33 stressMtx;
  OptionalArmaMat ionicForces;
  OptionalUInt numIters;

  void
  saveToStructure(common::Structure & structure) const;
  void
  loadFromStructure(const common::Structure & structure);
private:
  template< typename T>
    void
    setProperty(common::Structure & structure, utility::Key< T> & key,
        const boost::optional< T> & value) const;
  template< typename T>
    bool
    setOptimisationDataValue(boost::optional< T> & value,
        const common::Structure & structure,
        const utility::Key< T> & key) const;
};

class GeomOptimiser
{
public:

  virtual
  ~GeomOptimiser()
  {
  }

  // Get the potential being used by the geometry optimiser.  Not all
  // geometry optimisers need to have a potential in which case NULL
  // will be returned.
  virtual IPotential *
  getPotential()
  {
    return NULL;
  }
  virtual const IPotential *
  getPotential() const
  {
    return NULL;
  }

  virtual OptimisationOutcome
  optimise(common::Structure & structure,
      const OptimisationSettings & options) const = 0;
  virtual OptimisationOutcome
  optimise(common::Structure & structure, OptimisationData & data,
      const OptimisationSettings & options) const = 0;
};

inline void
OptimisationData::saveToStructure(common::Structure & structure) const
{
  namespace properties = common::structure_properties;

  setProperty(structure, properties::general::ENERGY_INTERNAL, internalEnergy);
  setProperty(structure, properties::general::ENTHALPY, enthalpy);
  setProperty(structure, properties::general::PRESSURE, pressure);
}

inline void
OptimisationData::loadFromStructure(const common::Structure & structure)
{
  namespace properties = common::structure_properties;

  setOptimisationDataValue(internalEnergy, structure,
      properties::general::ENERGY_INTERNAL);
  setOptimisationDataValue(enthalpy, structure, properties::general::ENTHALPY);
  setOptimisationDataValue(pressure, structure,
      properties::general::PRESSURE);
}

template< typename T>
  void
  OptimisationData::setProperty(common::Structure & structure,
      utility::Key< T> & key, const boost::optional< T> & value) const
  {
    if(value)
      structure.setProperty(key, *value);
    else
      structure.eraseProperty(key);
  }

template< typename T>
  bool
  OptimisationData::setOptimisationDataValue(boost::optional< T> & value,
      const common::Structure & structure, const utility::Key< T> & key) const
  {
    const T * structureValue = structure.getProperty(key);
    if(!structureValue)
      return false;

    value.reset(*structureValue);
    return true;
  }

}
}

#endif /* GEOM_OPTIMISER_H */
