/*
 * DefaultCrystalGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef DEFAULT_CRYSTAL_GENERATOR_H
#define DEFAULT_CRYSTAL_GENERATOR_H

// INCLUDES /////////////////////////////////////////////////////
#include "IStructureGenerator.h"

// FORWARD DECLARES /////////////////////////////////////////////
namespace sstbx {
namespace common {
class AtomGroup;
class Structure;
}
namespace build_cell {

class AbstractConstraintDescription;
class AtomGroupDescription;
class StructureBuilder;
class StructureDescription;
class StructureDescriptionMap;
}
}

namespace sstbx {
namespace build_cell {

class DefaultCrystalGenerator : public IStructureGenerator
{
public:

	DefaultCrystalGenerator(const bool useExtrudeMethod = false);
  virtual ~DefaultCrystalGenerator() {}

	/**
	 * Generate a cell based on the current set of constraints.
	 *
	 */
  virtual common::StructurePtr generateStructure(
    const StructureDescription &  strDesc,
    const common::AtomSpeciesDatabase & speciesDb,
    StructureGenerationOutcome::Value * outOutcome = NULL) const;

private:

  bool generateUnitCell(
    const StructureDescription & structureDescription,
    common::Structure & structure,
    const StructureBuilder & builder) const;

  StructureGenerationOutcome::Value generateAtomPositions(
	  StructureDescriptionMap & descriptionMap) const;

	/**
	/* The maximum number of times to attempt to create a structure before giving up.
	/**/
	const u32               myMaxAttempts;

  /**
  /* This option turns on using a method that extrudes any overlapping atoms after an initial
  /* random configuration has been generated.  Otherwise random configuration will be generated
  /* and checked for overlap, if an overlap is detected the configuration is thrown away and
  /* a new one generated.
  /**/
  const bool              myUseExtrudeMethod;
};

}}

#endif /* DEFAULT_CRYSTAL_GENERATOR_H */
