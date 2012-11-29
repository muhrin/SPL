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

::sstbx::utility::Key<unsigned int>       SPACEGROUP_NUMBER;
::sstbx::utility::Key< ::std::string>     SPACEGROUP_SYMBOL;
::sstbx::utility::Key<double>             PRESSURE_INTERNAL;
::sstbx::utility::Key<double>             ENERGY_INTERNAL;


} // namespace general

namespace searching {
::sstbx::utility::Key<unsigned int>       TIMES_FOUND;
}

namespace io {
::sstbx::utility::Key< ::boost::filesystem::path>  LAST_ABS_FILE_PATH;
}

} // namespace structure_properties
} // namespace common
} // namespace sstbx
