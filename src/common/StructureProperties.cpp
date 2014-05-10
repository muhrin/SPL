/*
 * StructureProperties.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "spl/common/StructureProperties.h"

namespace spl {
namespace common {
namespace structure_properties {
namespace general {

utility::NamedKey< unsigned int> SPACEGROUP_NUMBER("spaceGroupNo");
utility::NamedKey< std::string> SPACEGROUP_SYMBOL("spaceGroup");
utility::NamedKey< double> PRESSURE("pressure");
utility::NamedKey< double> ENERGY_INTERNAL("internalEnergy");
utility::NamedKey< double> ENTHALPY("enthalpy");
utility::NamedKey< double> FORMATION_ENTHALPY("formationEnthalpy");
utility::NamedKey< double> HULL_DISTANCE("hullDistance");
utility::Key< arma::mat33> STRESS_TENSOR;

} // namespace general

namespace searching {
utility::NamedKey< unsigned int> TIMES_FOUND("timesFound");
}

namespace io {
utility::Key< spl::io::ResourceLocator> LAST_ABS_FILE_PATH;
}

utility::NamedPropertyStore< utility::HeterogeneousMap> VISIBLE_PROPERTIES;

struct InitialiseVisibleProperties
{
  InitialiseVisibleProperties();
private:
  void
  add(utility::NamedProperty< utility::HeterogeneousMap> & property);
};
static InitialiseVisibleProperties init;

InitialiseVisibleProperties::InitialiseVisibleProperties()
{
  add(general::SPACEGROUP_NUMBER);
  add(general::SPACEGROUP_SYMBOL);
  add(general::PRESSURE);
  add(general::ENERGY_INTERNAL);
  add(general::ENTHALPY);
  add(general::FORMATION_ENTHALPY);
  add(searching::TIMES_FOUND);
}

void
InitialiseVisibleProperties::add(
    utility::NamedProperty< utility::HeterogeneousMap> & property)
{
  VISIBLE_PROPERTIES.addProperty(property);
}

} // namespace structure_properties
} // namespace common
} // namespace spl
