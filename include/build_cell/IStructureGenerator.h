/*
 * IStructureGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_GENERATOR_H
#define I_STRUCTURE_GENERATOR_H

// INCLUDES /////////////////////////////////
#include <utility>

#include "build_cell/StructureGenerationOutcome.h"
#include "common/Types.h"

// FORWARD DECLARES //////////////////////////
namespace sstbx {
namespace build_cell {
class StructureDescription;
}
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
}

namespace sstbx {
namespace build_cell {

class IStructureGenerator
{
public:

  virtual common::StructurePtr generateStructure(
    const StructureDescription &  strDesc,
    const common::AtomSpeciesDatabase & speciesDb,
    StructureGenerationOutcome::Value * outcome = NULL) const = 0;

  virtual ~IStructureGenerator() {}
};

}
}

#endif /* I_STRUCTURE_GENERATOR_H */
