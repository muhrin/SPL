/*
 * CrystalGenerator.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "build_cell/DefaultCrystalGenerator.h"

#include <memory>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "SSLibTypes.h"
#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/AtomGroupDescription.h"
#include "build_cell/AtomExtruder.h"
#include "build_cell/AtomsDescription.h"
#include "build_cell/DistanceConstraintChecker.h"
#include "build_cell/IUnitCellBlueprint.h"
#include "build_cell/RandomAtomPositioner.h"
#include "build_cell/StructureBuilder.h"
#include "build_cell/StructureDescription.h"
#include "build_cell/StructureDescriptionMap.h"
#include "build_cell/ConstVisitorGroup.h"
#include "common/Atom.h"
#include "common/Structure.h"
#include "common/Types.h"
#include "common/UnitCell.h"
#include "common/Utils.h"

namespace sstbx {
namespace build_cell {

namespace common = ::sstbx::common;

DefaultCrystalGenerator::DefaultCrystalGenerator(const bool useExtrudeMethod):
myUseExtrudeMethod(useExtrudeMethod),
myMaxAttempts(10000)
{
}


common::StructurePtr DefaultCrystalGenerator::generateStructure(
  const StructureDescription &  structureDescription,
  const common::AtomSpeciesDatabase & speciesDb,
  StructureGenerationOutcome::Value * outOutcome) const
{
	using ::sstbx::common::Structure;

  const bool hasUnitCell = structureDescription.getUnitCell() != NULL;

  // Create a builder that will populate the structure with the required atoms
  StructureBuilder builder(speciesDb);
  // and build
  StructureBuilder::DescriptionMapPtr descriptionMap;
  common::StructurePtr str = builder.buildStructure(structureDescription, descriptionMap);

  common::StructurePtr generatedStructure;
  StructureGenerationOutcome::Value outcome = StructureGenerationOutcome::SUCCESS;
	for(u32 i = 0; i < myMaxAttempts; ++i)
	{
    if(structureDescription.getUnitCell())
    {
      // Generate a unit cell for the structure
      if(!generateUnitCell(structureDescription, *str.get(), builder))
      {
        // That one failed, try again...
        outcome = StructureGenerationOutcome::FAILED_CREATING_UNIT_CELL;
        continue;
      }
    }

		// Genetate atom positions
    outcome = generateAtomPositions(*descriptionMap.get());
    if(outcome == StructureGenerationOutcome::SUCCESS)
		{
      generatedStructure = str;
			break;
		}
	}

  if(outOutcome)
    *outOutcome = outcome;

	return generatedStructure;
}

bool DefaultCrystalGenerator::generateUnitCell(
  const StructureDescription & structureDescription,
  common::Structure &  structure,
  const StructureBuilder & builder) const
{
  const IUnitCellBlueprint::StructureInfo structureInfo(structure.getNumAtoms(), builder.getAtomsVolume());
  common::UnitCellPtr cell;

  bool succeeded = false;
  for(u32 i = 0; i < myMaxAttempts; ++i)
  {
	  // Create a new unit cell
    cell = structureDescription.getUnitCell()->generateCell(structureInfo);

    // Check that none of the angles are very small
    if(cell->getNormVolume() > 0.1)
    {
      succeeded = true;
      break;
    }
  }

  if(succeeded)
    structure.setUnitCell(cell);

  return succeeded;
}

StructureGenerationOutcome::Value DefaultCrystalGenerator::generateAtomPositions(
  StructureDescriptionMap & descriptionMap) const
{
  StructureGenerationOutcome::Value outcome = StructureGenerationOutcome::SUCCESS;

  RandomAtomPositioner randomAtoms(descriptionMap);
  if(myUseExtrudeMethod)
  {
    descriptionMap.getStructureDescription().traversePostorder(randomAtoms);

    AtomExtruder extruder;

    if(!extruder.extrudeAtoms(descriptionMap.getStructure(), 800))
    {
      outcome = StructureGenerationOutcome::FAILED_SATISFYING_CONSTRAINTS;
    }
  }
  else
  {

    ConstVisitorGroup visitorGroup;

    DistanceConstraintChecker distanceConstraintsChecker(descriptionMap);

    visitorGroup.pushBack(randomAtoms);
    visitorGroup.pushBack(distanceConstraintsChecker);

    // Descent down the atom groups building everything and checking constraints
    if(!descriptionMap.getStructureDescription().traversePostorder(visitorGroup))
    {
      outcome = StructureGenerationOutcome::FAILED_SATISFYING_CONSTRAINTS;
    }
  }

  return outcome;
}



}
}
