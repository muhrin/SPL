///*
// * BlueprintStructureGenerator.cpp
// *
// *  Created on: Nov 10, 2011
// *      Author: Martin Uhrin
// */
//
//#include "build_cell/BlueprintStructureGenerator.h"
//
//#include <memory>
//
//#include <boost/foreach.hpp>
//#include <boost/shared_ptr.hpp>
//
//#include "SSLibTypes.h"
//#include "build_cell/AtomConstraintDescription.h"
//#include "build_cell/AtomGroupDescription.h"
//#include "build_cell/AtomExtruder.h"
//#include "build_cell/AtomsDescription.h"
//#include "build_cell/DistanceConstraintChecker.h"
//#include "build_cell/ICellGenerator.h"
//#include "build_cell/RandomAtomPositioner.h"
//#include "build_cell/RandomCellDescription.h"
//#include "build_cell/StructureBuilder.h"
//#include "build_cell/StructureDescription.h"
//#include "build_cell/StructureDescriptionMap.h"
//#include "build_cell/ConstVisitorGroup.h"
//#include "common/AbstractFmidCell.h"
//#include "common/Atom.h"
//#include "common/Structure.h"
//#include "common/Types.h"
//#include "common/Utils.h"
//
//namespace sstbx {
//namespace build_cell {
//
//namespace common = ::sstbx::common;
//
//BlueprintStructureGenerator::BlueprintStructureGenerator(
//  const ConstStructureBlueprintPtr blueprint,
//  const bool useExtrudeMethod):
//myStructureBlueprint(blueprint),
//myUseExtrudeMethod(useExtrudeMethod),
//myType(GeneratorType::CLUSTER),
//myMaxAttempts(10000)
//{
//}
//
//BlueprintStructureGenerator::BlueprintStructureGenerator(
//  const ConstStructureBlueprintPtr blueprint,
//	const ICellGenerator &	cellGenerator,
//  const bool useExtrudeMethod):
//myStructureBlueprint(blueprint),
//myCellGenerator(&cellGenerator),
//myUseExtrudeMethod(useExtrudeMethod),
//myType(GeneratorType::CELL_GENERATOR),
//myMaxAttempts(10000)
//{
//}
//
//
//IStructureGenerator::Result BlueprintStructureGenerator::generateStructure() const
//{
//	using ::sstbx::common::AbstractFmidCell;
//	using ::sstbx::common::Structure;
//
//  // Create a builder that will populate the structure with the required atoms
//  StructureBuilder builder;
//  // and build
//  StructureBuilder::StructurePair pair = builder.buildStructure(structureDescription);
//
//  // Make a copy of the cell description
//  RandomCellDescription localCellDesc(cellDesc);
//
//  // If a volume hasn't been set then try to use a calculated one
//  if(!localCellDesc.myVolume)
//  {
//    const double atomsVolume = builder.getAtomsVolume();
//    if(atomsVolume != 0.0)
//      localCellDesc.myVolume.reset(2.0 * builder.getAtomsVolume());
//  }
//
//  common::StructurePtr generatedStructure;
//  StructureGenerationOutcome::Value outcome = StructureGenerationOutcome::SUCCESS;
//	for(u32 i = 0; i < myMaxAttempts; ++i)
//	{
//    // Generate a unit cell for the structure
//    if(!generateUnitCell(localCellDesc, *pair.first.get()))
//    {
//      // That one failed, try again...
//      outcome = StructureGenerationOutcome::FAILED_CREATING_UNIT_CELL;
//      continue;
//    }
//
//		// Genetate atom positions
//    outcome = generateAtomPositions(*pair.second.get());
//    if(outcome == StructureGenerationOutcome::SUCCESS)
//		{
//      generatedStructure = pair.first;
//			break;
//		}
//	}
//
//	return Result(outcome, generatedStructure);
//}
//
//bool BlueprintStructureGenerator::generateUnitCell(
//  const RandomCellDescription & cellDesc,
//  ::sstbx::common::Structure &  structure) const
//{
//  common::UnitCellPtr cell;
//
//  bool succeeded = false;
//  for(u32 i = 0; i < myMaxAttempts; ++i)
//  {
//	  // Create a new unit cell
//    cell.reset(myCellGenerator.generateCell(cellDesc));
//
//    // Check that none of the angles are very small
//    if(cell->getNormVolume() > 0.1)
//    {
//      succeeded = true;
//      break;
//    }
//  }
//
//  if(succeeded)
//    structure.setUnitCell(cell);
//
//  return succeeded;
//}
//
//StructureGenerationOutcome::Value BlueprintStructureGenerator::generateAtomPositions(
//  StructureDescriptionMap & descriptionMap) const
//{
//  StructureGenerationOutcome::Value outcome = StructureGenerationOutcome::SUCCESS;
//
//  RandomAtomPositioner randomAtoms(descriptionMap);
//  if(myUseExtrudeMethod)
//  {
//    descriptionMap.getStructureDescription().traversePostorder(randomAtoms);
//
//    AtomExtruder extruder;
//
//    if(!extruder.extrudeAtoms(descriptionMap.getStructure()))
//    {
//      outcome = StructureGenerationOutcome::FAILED_SATISFYING_CONSTRAINTS;
//    }
//  }
//  else
//  {
//
//    ConstVisitorGroup visitorGroup;
//
//    DistanceConstraintChecker distanceConstraintsChecker(descriptionMap);
//
//    visitorGroup.pushBack(randomAtoms);
//    visitorGroup.pushBack(distanceConstraintsChecker);
//
//    // Descent down the atom groups building everything and checking constraints
//    if(!descriptionMap.getStructureDescription().traversePostorder(visitorGroup))
//    {
//      outcome = StructureGenerationOutcome::FAILED_SATISFYING_CONSTRAINTS;
//    }
//  }
//
//  return outcome;
//}
//
//
//
//}
//}
