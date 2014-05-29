## analysis

set(sslib_Source_Files__analysis
  src/analysis/AbsConvexHullGenerator.cpp
  src/analysis/AnchorArrangement.cpp
  src/analysis/ArrangementSmoothing.cpp
  src/analysis/ConvexHull.cpp
  src/analysis/ConvexHullStructures.cpp
  src/analysis/DynamicConvexHullGenerator.cpp
  src/analysis/GnuplotConvexHullPlotter.cpp
  src/analysis/Histogram.cpp
  src/analysis/SpaceGroup.cpp
  src/analysis/StructureConvexHullInfoSupplier.cpp
  src/analysis/StructureTriangulation.cpp
)
source_group("Source Files\\analysis" FILES ${sslib_Source_Files__analysis})

## build_cell

set(sslib_Source_Files__build_cell
  src/build_cell/AbsAtomsGenerator.cpp
  src/build_cell/AtomsGroup.cpp
  src/build_cell/BuildAtomInfo.cpp
  src/build_cell/GenBox.cpp
  src/build_cell/GenCylinder.cpp
  src/build_cell/GeneratorShape.cpp
  src/build_cell/GenSphere.cpp
  src/build_cell/PointGroups.cpp
  src/build_cell/PointSeparator.cpp
  src/build_cell/RandomUnitCellGenerator.cpp
  src/build_cell/StructureBuild.cpp
  src/build_cell/StructureBuilder.cpp
  src/build_cell/SymmetryFunctions.cpp
  src/build_cell/SymmetryGroup.cpp
  src/build_cell/SymmetryGroupBuilder.cpp
  src/build_cell/Utility.cpp
  src/build_cell/VoronoiSlabGenerator.cpp
  src/build_cell/VoronoiSlabRegions.cpp
)
source_group("Source Files\\build_cell" FILES ${sslib_Source_Files__build_cell})

## common

set(sslib_Source_Files__common
  src/common/Atom.cpp
  src/common/AtomGroup.cpp
  src/common/AtomsFormula.cpp
  src/common/AtomSpeciesDatabase.cpp
  src/common/AtomSpeciesInfo.cpp
  src/common/Constants.cpp
  src/common/DistanceCalculatorDelegator.cpp
  src/common/OrthoCellDistanceCalculator.cpp
  src/common/ReferenceDistanceCalculator.cpp
  src/common/Structure.cpp
  src/common/StructureProperties.cpp
  src/common/UnitCell.cpp
  src/common/UniversalCrystalDistanceCalculator.cpp
)
source_group("Source Files\\common" FILES ${sslib_Source_Files__common})

## common/event

set(sslib_Source_Files__common__event
)
source_group("Source Files\\common\\event" FILES ${sslib_Source_Files__common__event})


## factory

set(sslib_Source_Files__factory
  src/factory/GenShapeFactory.cpp
  src/factory/SsLibFactoryYaml.cpp
)
source_group("Source Files\\factory" FILES ${sslib_Source_Files__factory})

## io

set(sslib_Source_Files__io
  src/io/AtomFormatParser.cpp
  src/io/AtomYamlFormatParser.cpp
  src/io/BoostFilesystem.cpp
  src/io/CastepReader.cpp
  src/io/CellReaderWriter.cpp
  src/io/InfoLine.cpp
  src/io/Parsing.cpp
  src/io/ResourceLocator.cpp
  src/io/ResReaderWriter.cpp
  src/io/SplReaderWriter.cpp
  src/io/StructureYamlGenerator.cpp
  src/io/StructureReadWriteManager.cpp
  src/io/IoFunctions.cpp
  src/io/XyzReaderWriter.cpp
)
source_group("Source Files\\io" FILES ${sslib_Source_Files__io})

## math

set(sslib_Source_Files__math
  src/math/Geometry.cpp
  src/math/Matrix.cpp
  src/math/Random.cpp
  src/math/RunningStats.cpp
)
source_group("Source Files\\math" FILES ${sslib_Source_Files__math})

## os

set(sslib_Source_Files__os
  src/os/Process.cpp
)
source_group("Source Files\\os" FILES ${sslib_Source_Files__os})

## potential

set(sslib_Source_Files__potential
  src/potential/CastepGeomOptimiser.cpp
  src/potential/CastepRun.cpp
  src/potential/CombiningRules.cpp
  src/potential/ExternalOptimiser.cpp
  src/potential/LennardJones.cpp
  src/potential/PotentialData.cpp
  src/potential/StructureProperties.cpp
  src/potential/TpsdGeomOptimiser.cpp
)
source_group("Source Files\\potential" FILES ${sslib_Source_Files__potential})

## utility

set(sslib_Source_Files__utility
  src/utility/Armadillo.cpp
  src/utility/DistanceMatrixComparator.cpp
  src/utility/HeterogeneousMap.cpp
  src/utility/HeterogeneousMapKey.cpp
  src/utility/NamedProperty.cpp
  src/utility/Outcome.cpp
  src/utility/SortedDistanceComparator.cpp
  src/utility/SortedDistanceComparatorEx.cpp
  src/utility/StableComparison.cpp
  src/utility/UniqueStructureSet.cpp
  src/utility/UtilFunctions.cpp
)
source_group("Source Files\\utility" FILES ${sslib_Source_Files__utility})

## yaml

set(sslib_Source_Files__yaml
  src/yaml/ProgramOptions.cpp
  src/yaml/TranscodeFactory.cpp
  src/yaml/TranscodeGeneral.cpp
  src/yaml/TranscodeIo.cpp
)
source_group("Source Files\\yaml" FILES ${sslib_Source_Files__yaml})

set(sslib_Source_Files__
)
source_group("Header Files" FILES ${sslib_Source_Files__})

set(sslib_Source_Files
  ${sslib_Source_Files__analysis}
  ${sslib_Source_Files__build_cell}
  ${sslib_Source_Files__common}
  ${sslib_Source_Files__common__event}
  ${sslib_Source_Files__factory}
  ${sslib_Source_Files__io}
  ${sslib_Source_Files__math}
  ${sslib_Source_Files__os}
  ${sslib_Source_Files__potential}
  ${sslib_Source_Files__utility}
  ${sslib_Source_Files__yaml}
  ${sslib_Source_Files__}
)

