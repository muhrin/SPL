
## analysis

set(sslib_Header_Files__analysis
  include/spl/analysis/AbsConvexHullGenerator.h
  include/spl/analysis/AnchorArrangement.h
  include/spl/analysis/AnchorArrangementOutputter.h
  include/spl/analysis/ArrangementMapOutputter.h
  include/spl/analysis/ArrangementSmoothing.h
  include/spl/analysis/CgalCustomKernel.h
  include/spl/analysis/CgalPointDWithId.h
  include/spl/analysis/ConvexHull.h
  include/spl/analysis/ConvexHullInfoSupplier.h
  include/spl/analysis/ConvexHullOutputter.h
  include/spl/analysis/ConvexHullStructures.h
  include/spl/analysis/DynamicConvexHullGenerator.h
  include/spl/analysis/GnuplotAnchorArrangementPlotter.h
  include/spl/analysis/GnuplotConvexHullPlotter.h
  include/spl/analysis/Histogram.h
  include/spl/analysis/MapArrangementTraits.h
  include/spl/analysis/MatplotlibMapOutputter.h
  include/spl/analysis/SpaceGroup.h
  include/spl/analysis/StructureConvexHullInfoSupplier.h
  include/spl/analysis/StructureTriangulation.h
  include/spl/analysis/TraceCurve.h
  include/spl/analysis/VectorAnchorArrangementOutputter.h
  include/spl/analysis/VoronoiEdgeTracer.h
  include/spl/analysis/VoronoiPath.h
  include/spl/analysis/VoronoiPathArrangement.h
  include/spl/analysis/VoronoiPathDecompose.h
  include/spl/analysis/VoronoiPathTracer.h
  include/spl/analysis/VoronoiPathUtility.h
)
source_group("Header Files\\analysis" FILES ${sslib_Header_Files__analysis})

## analysis/detail

set(sslib_Header_Files__analysis__detail
  include/spl/analysis/detail/AnchorArrangement.h
  include/spl/analysis/detail/ArrangementSmoothing.h
  include/spl/analysis/detail/ConvexHullStructures.h
  include/spl/analysis/detail/GnuplotAnchorArrangementPlotter.h
  include/spl/analysis/detail/MatplotlibMapOutputter.h
  include/spl/analysis/detail/TraceCurve.h
  include/spl/analysis/detail/VectorAnchorArrangementOutputter.h
  include/spl/analysis/detail/VoronoiEdgeTracer.h
  include/spl/analysis/detail/VoronoiPath.h
  include/spl/analysis/detail/VoronoiPathArrangement.h
  include/spl/analysis/detail/VoronoiPathDecompose.h
  include/spl/analysis/detail/VoronoiPathTracer.h
  include/spl/analysis/detail/VoronoiPathUtility.h
)
source_group("Header Files\\analysis\\detail" FILES ${sslib_Header_Files__analysis__detail})

## build_cell

set(sslib_Header_Files__build_cell
  include/spl/build_cell/AbsAtomsGenerator.h
  include/spl/build_cell/AtomsDescription.h
  include/spl/build_cell/AtomsGroup.h
  include/spl/build_cell/AtomsGenerator.h
  include/spl/build_cell/BuildAtomInfo.h
  include/spl/build_cell/BuildCellFwd.h
  include/spl/build_cell/GenerationOutcome.h
  include/spl/build_cell/GenBox.h
  include/spl/build_cell/GenCylinder.h
  include/spl/build_cell/GenerationSettings.h
  include/spl/build_cell/GenSphere.h
  include/spl/build_cell/GeneratorShape.h
  include/spl/build_cell/IFragmentGenerator.h
  include/spl/build_cell/IUnitCellGenerator.h
  include/spl/build_cell/PointGroups.h
  include/spl/build_cell/PointSeparator.h
  include/spl/build_cell/RandomUnitCellGenerator.h
  include/spl/build_cell/Shapes.h
  include/spl/build_cell/StructureBuild.h
  include/spl/build_cell/StructureBuilder.h
  include/spl/build_cell/StructureContents.h
  include/spl/build_cell/StructureGenerator.h
  include/spl/build_cell/StructureGenerationOutcome.h
  include/spl/build_cell/SymmetryFunctions.h
  include/spl/build_cell/SymmetryGroup.h
  include/spl/build_cell/SymmetryGroupBuilder.h
  include/spl/build_cell/Utility.h
  include/spl/build_cell/VoronoiSlabGenerator.h
  include/spl/build_cell/VoronoiSlabRegions.h
)
source_group("Header Files\\build_cell" FILES ${sslib_Header_Files__build_cell})

## build_cell/detail

set(sslib_Header_Files__build_cell__detail
  include/spl/build_cell/detail/PointSeparator.h
)
source_group("Header Files\\build_cell\\detail" FILES ${sslib_Header_Files__build_cell__detail})

## common

set(sslib_Header_Files__common
  include/spl/common/Atom.h
  include/spl/common/AtomGroup.h
  include/spl/common/AtomsFormula.h
  include/spl/common/AtomSpeciesDatabase.h
  include/spl/common/AtomSpeciesId.h
  include/spl/common/AtomSpeciesInfo.h
  include/spl/common/ClusterDistanceCalculator.h
  include/spl/common/Constants.h
  include/spl/common/DistanceCalculator.h
  include/spl/common/DistanceCalculatorDelegator.h
  include/spl/common/OrthoCellDistanceCalculator.h
  include/spl/common/ReferenceDistanceCalculator.h
  include/spl/common/Structure.h
  include/spl/common/StructureListener.h
  include/spl/common/StructureProperties.h
  include/spl/common/Types.h
  include/spl/common/UnitCell.h
  include/spl/common/UniversalCrystalDistanceCalculator.h
)
source_group("Header Files\\common" FILES ${sslib_Header_Files__common})

## common/event

set(sslib_Header_Files__common__event
  include/spl/common/event/EventSupport.h
)
source_group("Header Files\\common\\event" FILES ${sslib_Header_Files__common__event})

## factory

set(sslib_Header_Files__factory
  include/spl/factory/FactoryError.h
  include/spl/factory/FactoryFwd.h
  include/spl/factory/GenShapeFactory.h
  include/spl/factory/SsLibFactoryYaml.h
  include/spl/factory/SsLibYamlSchema.h
)
source_group("Header Files\\factory" FILES ${sslib_Header_Files__factory})

## factory/detail

set(sslib_Header_Files__factory__detail
)
source_group("Header Files\\factory\\detail" FILES ${sslib_Header_Files__factory__detail})

## io

set(sslib_Header_Files__io
  include/spl/io/AtomFormatParser.h
  include/spl/io/AtomYamlFormatParser.h
  include/spl/io/BoostFilesystem.h
  include/spl/io/CastepReader.h
  include/spl/io/CellReaderWriter.h
  include/spl/io/InfoLine.h
  include/spl/io/IStructureReader.h
  include/spl/io/IStructureWriter.h
  include/spl/io/Parsing.h
  include/spl/io/ResourceLocator.h
  include/spl/io/ResReaderWriter.h
  include/spl/io/SplReaderWriter.h
  include/spl/io/StructureSchema.h
  include/spl/io/StructureYamlGenerator.h
  include/spl/io/StructureReadWriteManager.h
  include/spl/io/IoFunctions.h
  include/spl/io/XyzReaderWriter.h
)
source_group("Header Files\\io" FILES ${sslib_Header_Files__io})

set(sslib_Header_Files__io__detail
  include/spl/io/detail/AtomFormatParser.h
  include/spl/io/detail/StructureReadWriteManager.h
)
source_group("Header Files\\io\\detail" FILES ${sslib_Header_Files__io__detail})

## math
set(sslib_Header_Files__math
  include/spl/math/Geometry.h
  include/spl/math/LinearAlgebra.h
  include/spl/math/Matrix.h
  include/spl/math/NumberAlgorithms.h
  include/spl/math/Random.h
  include/spl/math/RunningStats.h
)
source_group("Header Files\\math" FILES ${sslib_Header_Files__math})

## math/detail
set(sslib_Header_Files__math__detail
  include/spl/math/detail/Random.h
)
source_group("Header Files\\math\\detail" FILES ${sslib_Header_Files__math__detail})

## os
set(sslib_Header_Files__os
  include/spl/os/Process.h
)
source_group("Header Files\\os" FILES ${sslib_Header_Files__os})

## potential

set(sslib_Header_Files__potential
  include/spl/potential/CastepGeomOptimiser.h
  include/spl/potential/CastepRun.h
  include/spl/potential/CombiningRules.h
  include/spl/potential/ExternalOptimiser.h
  include/spl/potential/GenericPotentialEvaluator.h
  include/spl/potential/GeomOptimiser.h
  include/spl/potential/IParameterisable.h
  include/spl/potential/IPotential.h
  include/spl/potential/IPotentialEvaluator.h
  include/spl/potential/LennardJones.h
  include/spl/potential/OptimisationSettings.h
  include/spl/potential/PotentialData.h
  include/spl/potential/StructureProperties.h
  include/spl/potential/TpsdGeomOptimiser.h
  include/spl/potential/Types.h
)
source_group("Header Files\\potential" FILES ${sslib_Header_Files__potential})

## utility

set(sslib_Header_Files__utility
  include/spl/utility/Armadillo.h
  include/spl/utility/DataPool.h
  include/spl/utility/DistanceMatrixComparator.h
  include/spl/utility/EdgeMap.h
  include/spl/utility/GenericBufferedComparator.h
  include/spl/utility/HeterogeneousMap.h
  include/spl/utility/HeterogeneousMapEx.h
  include/spl/utility/HeterogeneousMapKey.h
  include/spl/utility/IBufferedComparator.h
  include/spl/utility/IndexingEnums.h
  include/spl/utility/IndexRemappingView.h
  include/spl/utility/IStructureComparator.h
  include/spl/utility/Iterator.h
  include/spl/utility/MapEx.h
  include/spl/utility/MetaProgramming.h
  include/spl/utility/MultiArray.h
  include/spl/utility/MultiIdx.h
  include/spl/utility/MultiIdxRange.h
  include/spl/utility/MultiIterator.h
  include/spl/utility/MultiRange.h
  include/spl/utility/NamedProperty.h
  include/spl/utility/Outcome.h
  include/spl/utility/PermutationRange.h
  include/spl/utility/PromotableType.h
  include/spl/utility/PropertiesObject.h
  include/spl/utility/Range.h
  include/spl/utility/SharedHandle.h
  include/spl/utility/SortedDistanceComparator.h
  include/spl/utility/SortedDistanceComparatorEx.h
  include/spl/utility/StableComparison.h
  include/spl/utility/TransformFunctions.h
  include/spl/utility/TypedDataTable.h
  include/spl/utility/UniqueStructureSet.h
  include/spl/utility/UtilFunctions.h
  include/spl/utility/UtilityFwd.h
)
source_group("Header Files\\utility" FILES ${sslib_Header_Files__utility})

## utility/detail

set(sslib_Header_Files__utility__detail
  include/spl/utility/detail/DataPool.h
  include/spl/utility/detail/NamedProperty.h
  include/spl/utility/detail/TypedDataTable.h
  include/spl/utility/detail/GenericBufferedComparator.h
  include/spl/utility/detail/HeterogeneousMap.h
  include/spl/utility/detail/HeterogeneousMapKey.h
  include/spl/utility/detail/MultiIterator.h
  include/spl/utility/detail/MultiRange.h
  include/spl/utility/detail/SharedHandle.h
  include/spl/utility/detail/UniqueStructureSet.h
)
source_group("Header Files\\utility\\detail" FILES ${sslib_Header_Files__utility__detail})

## yaml

set(sslib_Header_Files__yaml
  include/spl/yaml/HelperTypes.h
  include/spl/yaml/ProgramOptions.h
  include/spl/yaml/Transcode.h
  include/spl/yaml/TranscodeFactory.h
  include/spl/yaml/TranscodeGeneral.h
  include/spl/yaml/TranscodeIo.h
)
source_group("Header Files\\yaml" FILES ${sslib_Header_Files__yaml})

## yaml/detail

set(sslib_Header_Files__yaml__detail
  include/spl/yaml/detail/TranscodeGeneral.h
)
source_group("Header Files\\yaml\\detail" FILES ${sslib_Header_Files__yaml__detail})

set(sslib_Header_Files__
  include/spl/OptionalTypes.h
  include/spl/SSLibConfig.h.in
  ${PROJECT_BINARY_DIR}/include/spl/SSLibConfig.h
  include/spl/SSLibAssert.h
  include/spl/SSLib.h
  include/spl/Types.h
)
source_group("Header Files" FILES ${sslib_Header_Files__})

set(sslib_Header_Files
  ${sslib_Header_Files__analysis}
  ${sslib_Header_Files__analysis__detail}
  ${sslib_Header_Files__build_cell}
  ${sslib_Header_Files__common}
  ${sslib_Header_Files__common__event}
  ${sslib_Header_Files__factory}
  ${sslib_Header_Files__factory__detail}
  ${sslib_Header_Files__io}
  ${sslib_Header_Files__io__detail}
  ${sslib_Header_Files__math}
  ${sslib_Header_Files__math__detail}
  ${sslib_Header_Files__os}
  ${sslib_Header_Files__potential}
  ${sslib_Header_Files__utility}
  ${sslib_Header_Files__utility__detail}
  ${sslib_Header_Files__yaml}
  ${sslib_Header_Files__yaml__detail}
  ${sslib_Header_Files__}
)

