/*
 * StructureProperties.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_PROPERTIES_H
#define STRUCTURE_PROPERTIES_H

// INCLUDES ///////////////////////////////////////////////
#include "SSLib.h"

#include <boost/filesystem.hpp>

#include "utility/HeterogeneousMap.h"
#include "utility/NamedProperty.h"


namespace sstbx {

// FORWARD DECLARES ////////////////////////////
namespace io {
class ResourceLocator;
}

namespace common {

namespace structure_properties {
namespace general {

extern utility::NamedKey<unsigned int>       SPACEGROUP_NUMBER;
extern utility::NamedKey< ::std::string>     SPACEGROUP_SYMBOL;
extern utility::NamedKey<double>             PRESSURE_INTERNAL;
extern utility::NamedKey<double>             ENERGY_INTERNAL;


} // namespace general

namespace searching {
extern utility::NamedKey<unsigned int>       TIMES_FOUND;
}

namespace io {
extern utility::Key< ::sstbx::io::ResourceLocator>  LAST_ABS_FILE_PATH;
}

extern utility::NamedPropertyStore<utility::HeterogeneousMap> VISIBLE_PROPERTIES;

} // namespace structure_properties
} // namespace common
} // namespace sstbx

#endif /* STRUCTURE_PROPERTIES_H */
