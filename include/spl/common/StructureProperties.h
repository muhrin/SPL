/*
 * StructureProperties.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_PROPERTIES_H
#define STRUCTURE_PROPERTIES_H

// INCLUDES ///////////////////////////////////////////////
#include "spl/SSLib.h"

#include <boost/filesystem.hpp>

#include <armadillo>

#include "spl/utility/HeterogeneousMap.h"
#include "spl/utility/NamedProperty.h"

namespace spl {

// FORWARD DECLARES ////////////////////////////
namespace io {
class ResourceLocator;
}

namespace common {
namespace structure_properties {
namespace general {

extern utility::NamedKey< unsigned int> SPACEGROUP_NUMBER;
extern utility::NamedKey< std::string> SPACEGROUP_SYMBOL;
extern utility::NamedKey< double> PRESSURE;
extern utility::NamedKey< double> ENERGY_INTERNAL;
extern utility::NamedKey< double> ENTHALPY;
extern utility::NamedKey< double> FORMATION_ENTHALPY;
extern utility::NamedKey< double> HULL_DISTANCE;
extern utility::Key< arma::mat33> STRESS_TENSOR;

} // namespace general

namespace searching {
extern utility::NamedKey< unsigned int> TIMES_FOUND;
}

namespace io {
extern utility::Key< spl::io::ResourceLocator> LAST_ABS_FILE_PATH;
}

extern utility::NamedPropertyStore< utility::HeterogeneousMap> VISIBLE_PROPERTIES;

} // namespace structure_properties
} // namespace common
} // namespace spl

#endif /* STRUCTURE_PROPERTIES_H */
