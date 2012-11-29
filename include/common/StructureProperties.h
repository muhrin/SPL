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

#include "utility/HeterogeneousMapKey.h"


namespace sstbx {
namespace common {
namespace structure_properties {
namespace general {

extern ::sstbx::utility::Key<unsigned int>       SPACEGROUP_NUMBER;
extern ::sstbx::utility::Key< ::std::string>     SPACEGROUP_SYMBOL;
extern ::sstbx::utility::Key<double>             PRESSURE_INTERNAL;
extern ::sstbx::utility::Key<double>             ENERGY_INTERNAL;


} // namespace general

namespace searching {
extern ::sstbx::utility::Key<unsigned int>       TIMES_FOUND;
}

namespace io {
extern ::sstbx::utility::Key< ::boost::filesystem::path>  LAST_ABS_FILE_PATH;
}

} // namespace structure_properties
} // namespace common
} // namespace sstbx

#endif /* STRUCTURE_PROPERTIES_H */
