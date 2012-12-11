/*
 * StructureProperties.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/StructureProperties.h"

namespace sstbx {
namespace common {
namespace structure_properties {
namespace general {

utility::NamedKey<unsigned int>       SPACEGROUP_NUMBER("spaceGroupNo");
utility::NamedKey< ::std::string>     SPACEGROUP_SYMBOL("spaceGroup");
utility::NamedKey<double>             PRESSURE_INTERNAL("pressure");
utility::NamedKey<double>             ENERGY_INTERNAL("internalEnergy");


} // namespace general

namespace searching {
utility::NamedKey<unsigned int>       TIMES_FOUND("timesFound");
}

namespace io {
  utility::Key< ::sstbx::io::ResourceLocator>  LAST_ABS_FILE_PATH;
}

utility::NamedPropertyStore<utility::HeterogeneousMap> VISIBLE_PROPERTIES;

struct InitialiseVisibleProperties
{
  InitialiseVisibleProperties();
private:
  void add(utility::NamedProperty<utility::HeterogeneousMap> & property);
};
static InitialiseVisibleProperties init;

InitialiseVisibleProperties::InitialiseVisibleProperties()
{
  add(general::SPACEGROUP_NUMBER);
  add(general::SPACEGROUP_SYMBOL);
  add(general::PRESSURE_INTERNAL);
  add(general::ENERGY_INTERNAL);
  add(searching::TIMES_FOUND);
}


void InitialiseVisibleProperties::add(utility::NamedProperty<utility::HeterogeneousMap> & property)
{
  VISIBLE_PROPERTIES.addProperty(property);
}

} // namespace structure_properties
} // namespace common
} // namespace sstbx
