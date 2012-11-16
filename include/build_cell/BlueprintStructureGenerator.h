///*
// * BlueprintStructureGenerator.h
// *
// *  Created on: Aug 17, 2011
// *      Author: Martin Uhrin
// */
//
//#ifndef BLUEPRINT_STRUCTURE_GENERATOR_H
//#define BLUEPRINT_STRUCTURE_GENERATOR_H
//
//// INCLUDES /////////////////////////////////
//#include <utility>
//
//#include "build_cell/IStructureGenerator.h"
//#include "build_cell/StructureGenerationOutcome.h"
//#include "common/Types.h"
//
//// FORWARD DECLARES /////////////////////////////////////////////
//namespace sstbx {
//namespace common {
//class AbstractFmidCell;
//class AtomGroup;
//class Structure;
//}
//namespace build_cell {
//class AbstractConstraintDescription;
//class AtomGroupDescription;
//class ICellGenerator;
//class RandomCellDescription;
//class StructureBuilder;
//class StructureDescription;
//class StructureDescriptionMap;
//}
//}
//
//class BlueprintStructureGenerator : public IStructureGenerator
//{
//public:
//  typedef ::boost::shared_ptr<const StructureDescription> ConstStructureBlueprintPtr;
//  typedef IStructureGenerator::Result Result;
//
//  /**
//  /* Create structure generator for a cluster (i.e. no unit cell).
//  /**/
//  BlueprintStructureGenerator(const ConstStructureBlueprintPtr blueprint, const bool useExtrudeMethod = false);
//
//  /**
//  /* Create structure generator for a structure with a unit cell.
//  /**/
//	BlueprintStructureGenerator(const ConstStructureBlueprintPtr blueprint, const ICellGenerator & cellGenerator, const bool useExtrudeMethod = false);
//
//
//  ConstStructureBlueprintPtr getStructureBlueprint() const;
//
//
//  // From IStructureGenerator /////////////////
//
//	/**
//	 * Generate a new structure.
//	 *
//	 */
//  virtual Result generateStructure() const;
//
//  // End from IStructureGenerator /////////////
//
//private:
//
//  struct GeneratorType
//  {
//  public:
//    enum Value
//    {
//      CLUSTER,        // No unit cell, structure is cluster
//      CELL_GENERATOR, // Generate new unit cell each time
//      FIXED_CELL      // Use a fixed unit cell for all structures
//    };
//  };
//
//  const GeneratorType::Value myType;
//
//  const ConstStructureBlueprintPtr  myStructureBlueprint;
//
//	/**
//	/* The maximum number of times to attempt to create a structure before giving up.
//	/**/
//	const u32               myMaxAttempts;
//
//  /**
//  /* This option turns on using a method that extrudes any overlapping atoms after an initial
//  /* random configuration has been generated.  Otherwise random configuration will be generated
//  /* and checked for overlap, if an overlap is detected the configuration is thrown away and
//  /* a new one generated.
//  /**/
//  const bool              myUseExtrudeMethod;
//
//	/**
//	 * The generator used the create the cell for the crystal.
//	 */
//	const ICellGenerator * myCellGenerator;
//
//	::sstbx::common::Structure * const generateStructure(const ::sstbx::common::AbstractFmidCell * const cell) const;
//
//  bool generateUnitCell(
//    const RandomCellDescription & cellDesc,
//    ::sstbx::common::Structure &  structure) const;
//
//  StructureGenerationOutcome::Value generateAtomPositions(
//	  StructureDescriptionMap & descriptionMap) const;
//};
//
//}
//}
//
//#endif /* BLUEPRINT_STRUCTURE_GENERATOR_H */
