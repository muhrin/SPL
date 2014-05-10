/*
 * StructureGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_GENERATOR_H
#define STRUCTURE_GENERATOR_H

// INCLUDES /////////////////////////////////

#include "spl/common/Types.h"

#include "spl/build_cell/GenerationOutcome.h"

namespace spl {

// FORWARD DECLARES //////////////////////////
namespace common {
class AtomSpeciesDatabase;
class Structure;
}

namespace build_cell {
class GenerationSettings;

class StructureGenerator
{
public:
  virtual
  ~StructureGenerator()
  {
  }

  virtual GenerationOutcome
  generateStructure(common::StructurePtr & structureOut,
      const common::AtomSpeciesDatabase & speciesDb) = 0;
  virtual GenerationOutcome
  generateStructure(common::StructurePtr & structureOut,
      const common::AtomSpeciesDatabase & speciesDb,
      const GenerationSettings & info) = 0;
};

}
}

#endif /* STRUCTURE_GENERATOR_H */
