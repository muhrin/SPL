/*
 * SsLibYamlSchema.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_YAML_SCHEMA_H
#define SSLIB_YAML_SCHEMA_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_YAML

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <schemer/Schemer.h>

#include "spl/build_cell/VoronoiSlabRegions.h"
#include "spl/potential/CombiningRules.h"
#include "spl/potential/TpsdGeomOptimiser.h"
#include "spl/utility/SortedDistanceComparator.h"
#include "spl/yaml/Transcode.h"

// DEFINES //////////////////////////////////////////////

namespace spl {
namespace factory {

// SIMPLE SCHEMA TYPES //////////////////////////////////////
typedef schemer::Scalar< yaml::VecAsString< std::string>::Type> StringsVector;
typedef schemer::Scalar< yaml::ArmaTriangularMat> TriangularMatrix;
typedef schemer::Scalar< arma::rowvec2> Rowvec2;
typedef schemer::Scalar< arma::rowvec3> Rowvec3;
typedef schemer::Scalar< arma::rowvec4> Rowvec4;
typedef schemer::Scalar< AtomSpeciesCount> AtomSpeciesCountScalar;
typedef schemer::Scalar< yaml::VecAsString< utility::Range< double> >::Type> DoubleRanges;
typedef schemer::Scalar< SpeciesPair> SpeciesPairSchema;

struct MinMax
{
  boost::optional< double> min;
  boost::optional< double> max;
};

SCHEMER_MAP(MinMaxSchema, MinMax)
{
  element("min", &MinMax::min);
  element("max", &MinMax::max);
}

// POTENTIALS ////////////////////////////////////////////////

SCHEMER_LIST(LjParams, schemer::Scalar<double>)
{
  length(5);
}

SCHEMER_HOMO_MAP_KEY_TYPED(LjInteraction, schemer::Scalar<SpeciesPair>, LjParams)
{
}

struct LennardJones
{
  std::map< SpeciesPair, std::vector< double> > params;
  potential::CombiningRule::Value epsilonCombiningRule;
  potential::CombiningRule::Value sigmaCombiningRule;
};

SCHEMER_ENUM(CombiningRuleSchema, potential::CombiningRule::Value)
{
  enumeration("none", potential::CombiningRule::NONE);
  enumeration("lorentz", potential::CombiningRule::LORENTZ);
  enumeration("berthelot", potential::CombiningRule::BERTHELOT);
  enumeration("lj", potential::CombiningRule::UHRIN_PICKARD);
}

SCHEMER_MAP(LennardJonesSchema, LennardJones)
{
  element< LjInteraction>("params", &LennardJones::params);
  element< CombiningRuleSchema>("epsilonCombining",
      &LennardJones::epsilonCombiningRule)->defaultValue(
      potential::CombiningRule::NONE);
  element< CombiningRuleSchema>("sigmaCombining",
      &LennardJones::sigmaCombiningRule)->defaultValue(
      potential::CombiningRule::NONE);
}

struct Potential
{
  boost::optional< LennardJones> lj;
};

SCHEMER_MAP(PotentialSchema, Potential)
{
  element("lennardJones", &Potential::lj);
}

// OPTIMISERS ////////////////////////////////////////////////
struct OptimiserSettings
{
  int maxIterations;
  double energyTolerance;
  double forceTolerance;
  double stressTolerance;
  double pressure;
};

SCHEMER_MAP(OptimiserSettingsSchema, OptimiserSettings)
{
  element("maxIter", &OptimiserSettings::maxIterations)->defaultValue(
      potential::TpsdGeomOptimiser::DEFAULT_MAX_ITERATIONS);
  element("energyTol", &OptimiserSettings::energyTolerance)->defaultValue(
      potential::TpsdGeomOptimiser::DEFAULT_ENERGY_TOLERANCE);
  element("forceTol", &OptimiserSettings::forceTolerance)->defaultValue(
      potential::TpsdGeomOptimiser::DEFAULT_FORCE_TOLERANCE);
  element("stressTol", &OptimiserSettings::stressTolerance)->defaultValue(
      potential::TpsdGeomOptimiser::DEFAULT_STRESS_TOLERANCE);
  element("pressure", &OptimiserSettings::pressure)->defaultValue(0.0);
}

struct OptimiserWithPotentialSettings
{
  Potential potential;
};

SCHEMER_MAP(OptimiserWithPotentialSettingsSchema, OptimiserWithPotentialSettings)
{
  element("potential", &OptimiserWithPotentialSettings::potential);
}

struct Castep
{
  std::string runCommand;
  std::string seed;
  bool keepIntermediates;
  int numRoughSteps;
  int numSelfConsistent;
};

SCHEMER_MAP(CastepSchema, Castep)
{
  element("exe", &Castep::runCommand)->defaultValue("castep");
  element("seed", &Castep::seed);
  element("keep", &Castep::keepIntermediates)->defaultValue(false);
  element("roughSteps", &Castep::numRoughSteps)->defaultValue(0);
  element("consistent", &Castep::numSelfConsistent)->defaultValue(2);
}

struct ExternalOptimiser
{
  std::string exe;
};

SCHEMER_MAP(ExternalOptimiserType, ExternalOptimiser)
{
  element("exe", &ExternalOptimiser::exe);
}

struct Optimiser
{
  boost::optional< Castep> castep;
  boost::optional< ExternalOptimiser> external;
  boost::optional< OptimiserWithPotentialSettings> tpsd;
};

SCHEMER_MAP(OptimiserSchema, Optimiser)
{
  element("castep", &Optimiser::castep);
  element("external", &Optimiser::external);
  element("tpsd", &Optimiser::tpsd);
}

// STRUCTURE //////////////////////////////////////
struct UnitCell
{
  std::vector< double> abc;
};

SCHEMER_LIST(LatticeParams, schemer::Scalar<double>)
{
  length(6);
}

SCHEMER_MAP(UnitCellSchema, UnitCell)
{
  element< LatticeParams>("abc", &UnitCell::abc);
}

struct AtomsData
{
  AtomSpeciesCount species;
};

SCHEMER_MAP(AtomsDataSchema, AtomsData)
{
  element< AtomSpeciesCountScalar>("spec", &AtomsData::species);
}

// STRUCTURE BUILDER //////////////////////////////

namespace builder {

struct GenBox
{
  boost::optional< arma::rowvec3> pos;
  boost::optional< arma::rowvec4> rot;
  boost::optional< double> shellThickness;
  double width;
  double height;
  double depth;
};

SCHEMER_MAP(GenBoxSchema, GenBox)
{
  element< Rowvec3>("pos", &GenBox::pos)->defaultValue(
      arma::zeros< arma::rowvec>(3));
  element< Rowvec4>("rot", &GenBox::rot);
  element("shell", &GenBox::shellThickness);
  element("width", &GenBox::width);
  element("height", &GenBox::height);
  element("depth", &GenBox::depth);
}

struct GenCylinder
{
  double radius;
  double height;
  boost::optional< arma::rowvec3> pos;
  boost::optional< arma::rowvec4> rot;
  boost::optional< double> shellThickness;
  boost::optional< bool> shellCapped;
};

SCHEMER_MAP(GenCylinderSchema, GenCylinder)
{
  element("radius", &GenCylinder::radius);
  element("height", &GenCylinder::height);
  element< Rowvec3>("pos", &GenCylinder::pos)->defaultValue(
      arma::zeros< arma::rowvec>(3));
  element< Rowvec4>("rot", &GenCylinder::rot);
  element("shell", &GenCylinder::shellThickness);
  element("shellCapped", &GenCylinder::shellCapped);
}

struct GenSphere
{
  double radius;
  boost::optional< arma::rowvec3> pos;
  boost::optional< arma::rowvec4> rot;
  boost::optional< double> shellThickness;
};

SCHEMER_MAP(GenSphereSchema, GenSphere)
{
  element("radius", &GenSphere::radius);
  element< Rowvec3>("pos", &GenSphere::pos)->defaultValue(
      arma::zeros< arma::rowvec>(3));
  element< Rowvec4>("rot", &GenSphere::rot)->defaultValue(
      arma::zeros< arma::rowvec>(4));
  element("shell", &GenSphere::shellThickness);
}

struct GenShape
{
  boost::optional< GenBox> box;
  boost::optional< GenCylinder> cylinder;
  boost::optional< GenSphere> sphere;
};

SCHEMER_MAP(GenShapeSchema, GenShape)
{
  element("box", &GenShape::box);
  element("cylinder", &GenShape::cylinder);
  element("sphere", &GenShape::sphere);
}

struct MinMaxRatio
{
  boost::optional< double> min;
  boost::optional< double> max;
  boost::optional< double> maxRatio;
};

SCHEMER_MAP(MinMaxRatioSchema, MinMaxRatio)
{
  element("min", &MinMaxRatio::min);
  element("max", &MinMaxRatio::max);
  element("maxRatio", &MinMaxRatio::maxRatio);
}

struct UnitCellBuilder
{
  typedef utility::Range< double> DoubleRange;
  boost::optional< std::vector< DoubleRange> > abc;
  boost::optional< double> vol;
  boost::optional< double> delta;
  boost::optional< double> mul;
  boost::optional< MinMaxRatio> lengths;
  boost::optional< MinMaxRatio> angles;
};

SCHEMER_MAP(UnitCellBuilderSchema, UnitCellBuilder)
{
  element< DoubleRanges>("abc", &UnitCellBuilder::abc);
  element("vol", &UnitCellBuilder::vol);
  element("delta", &UnitCellBuilder::delta);
  element("mul", &UnitCellBuilder::mul);
  element("lengths", &UnitCellBuilder::lengths);
  element("angles", &UnitCellBuilder::angles);
}

typedef std::vector< std::string> AtomsCompactInfo;

struct SimpleAtomsData
{
  AtomSpeciesCount species;
  boost::optional< AtomsCompactInfo> info;
  boost::optional< double> radius;
  boost::optional< arma::rowvec3> pos;
  boost::optional< std::string> label;
};

SCHEMER_MAP(SimpleAtomsDataSchema, SimpleAtomsData)
{
  element< AtomSpeciesCountScalar>("spec", &SimpleAtomsData::species);
  element("info", &SimpleAtomsData::info);
  element("radius", &SimpleAtomsData::radius);
  element< Rowvec3>("pos", &SimpleAtomsData::pos);
  element("label", &SimpleAtomsData::label);
}

typedef schemer::VariantListMap< schemer::List< schemer::String>,
    SimpleAtomsDataSchema> SimpleAtomSpec;
typedef schemer::List< SimpleAtomSpec> SimpleAtomList;

typedef boost::variant< AtomsCompactInfo, SimpleAtomsData> SimpleAtomsDataEntry;

struct AtomsGroup
{
  boost::optional< GenShape> genShape;
  std::vector< SimpleAtomsDataEntry> atoms;
  boost::optional< double> atomsRadius;
  boost::optional< arma::rowvec3> pos;
  boost::optional< arma::rowvec4> rot;
  int num;
  boost::optional< PairDistances> pairDistances;
};

SCHEMER_MAP(AtomsGroupSchema, AtomsGroup)
{
  element("genShape", &AtomsGroup::genShape);
  element< SimpleAtomList>("atoms", &AtomsGroup::atoms);
  element("atomsRadius", &AtomsGroup::atomsRadius);
  element< Rowvec3>("pos", &AtomsGroup::pos);
  element< Rowvec4>("rot", &AtomsGroup::rot);
  element("num", &AtomsGroup::num)->defaultValue(1);
  element("pairDistances", &AtomsGroup::pairDistances);
}

struct Symmetry
{
  boost::optional< MinMax> minMaxOps;
  boost::optional< std::string> pointGroup;
};

SCHEMER_MAP(SymmetrySchema, Symmetry)
{
  element("ops", &Symmetry::minMaxOps);
  element("pointGroup", &Symmetry::pointGroup);
}

struct Builder
{
  boost::optional< std::vector< std::string> > atomsFormat;
  boost::optional< double> atomsRadius;
  boost::optional< bool> cluster;
  boost::optional< std::vector< SimpleAtomsDataEntry> > atoms;
  boost::optional< std::vector< AtomsGroup> > groups;
  boost::optional< GenShape> genShape;
  boost::optional< UnitCellBuilder> unitCellBuilder;
  boost::optional< Symmetry> symmetry;
  boost::optional< std::map< std::string, double> > pairDistances;
  boost::optional< double> overlap;
};

SCHEMER_MAP(BuilderSchema, Builder)
{
  element("atomsFormat", &Builder::atomsFormat);
  element("atomsRadius", &Builder::atomsRadius);
  element("cluster", &Builder::cluster)->defaultValue(false);
  element< SimpleAtomList>("atoms", &Builder::atoms);
  element("groups", &Builder::groups);
  element("genShape", &Builder::genShape);
  element("unitCell", &Builder::unitCellBuilder);
  element("symmetry", &Builder::symmetry);
  element("pairDistances", &Builder::pairDistances);
  element("overlap", &Builder::overlap);
}

#ifdef SPL_WITH_CGAL

struct VoronoiSlabRegionBasis
{
  boost::optional< std::vector< std::string> > ordered;
  boost::optional< std::map< std::string, int> > random;
};

SCHEMER_MAP(VoronoiSlabRegionBasisType, VoronoiSlabRegionBasis)
{
  element("ordered", &VoronoiSlabRegionBasis::ordered);
  element("random", &VoronoiSlabRegionBasis::random);
}

struct VoronoiSlabLatticeRegion
{
  std::vector< arma::rowvec2> boundary;
  std::vector< arma::rowvec2> lattice;
  boost::optional< arma::rowvec2> startPoint;
  bool fixPoints;
};

SCHEMER_LIST(Lattice2DType, Rowvec2)
{
  length(2);
}

SCHEMER_MAP(VoronoiSlabLatticeRegionType, VoronoiSlabLatticeRegion)
{
  element("boundary", &VoronoiSlabLatticeRegion::boundary);
  element< Lattice2DType>("lattice", &VoronoiSlabLatticeRegion::lattice);
  element< Rowvec2>("startPoint", &VoronoiSlabLatticeRegion::startPoint);
  element("fixPoints", &VoronoiSlabLatticeRegion::fixPoints)->defaultValue(
      true);
}

SCHEMER_ENUM(VoronoiSlabRandomRegionPolyModeType, build_cell::RandomRegion::PolyMode::Value)
{
  enumeration("free", build_cell::RandomRegion::PolyMode::FREE);
  enumeration("stochastic", build_cell::RandomRegion::PolyMode::STOCHASTIC);
  enumeration("fixed", build_cell::RandomRegion::PolyMode::FIXED);
}

struct VoronoiSlabRandomRegion
{
  std::vector< arma::rowvec2> boundary;
  int numPoints;
  double minsep;
  build_cell::RandomRegion::PolyMode::Value polyMode;
  boost::optional< std::map< int, int> > polys;
};

SCHEMER_MAP(VoronoiSlabRandomRegionType, VoronoiSlabRandomRegion)
{
  element("boundary", &VoronoiSlabRandomRegion::boundary);
  element("numPoints", &VoronoiSlabRandomRegion::numPoints);
  element("minsep", &VoronoiSlabRandomRegion::minsep);
  element< VoronoiSlabRandomRegionPolyModeType>("polyMode",
      &VoronoiSlabRandomRegion::polyMode)->defaultValue(
      build_cell::RandomRegion::PolyMode::FREE);
  element("polys", &VoronoiSlabRandomRegion::polys);
}

struct VoronoiSlabRegion
{
  VoronoiSlabRegionBasis basis;
  boost::optional< VoronoiSlabLatticeRegion> lattice;
  boost::optional< VoronoiSlabRandomRegion> random;
};

SCHEMER_MAP(VoronoiSlabRegionType, VoronoiSlabRegion)
{
  element("basis", &VoronoiSlabRegion::basis);
  element("lattice", &VoronoiSlabRegion::lattice);
  element("random", &VoronoiSlabRegion::random);
}

struct VoronoiSlab
{
  boost::optional< arma::rowvec3> pos;
  boost::optional< arma::rowvec4> rot;
  std::vector< VoronoiSlabRegion> regions;
  boost::optional< bool> debugSaveTriangulation;
};

SCHEMER_MAP(VoronoiSlabType, VoronoiSlab)
{
  element< Rowvec3>("pos", &VoronoiSlab::pos);
  element< Rowvec4>("rot", &VoronoiSlab::rot);
  element("regions", &VoronoiSlab::regions);
  element("debugSaveTriangulation", &VoronoiSlab::debugSaveTriangulation);
}

struct VoronoiSlabGenerator
{
  std::vector< VoronoiSlab> slabs;
  boost::optional< UnitCell> unitCell;
};

SCHEMER_MAP(VoronoiSlabGeneratorType, VoronoiSlabGenerator)
{
  element("slabs", &VoronoiSlabGenerator::slabs);
  element("unitCell", &VoronoiSlabGenerator::unitCell);
}

#endif // SPL_WITH_CGAL

struct StructureGenerator
{
  boost::optional< Builder> builder;
#ifdef SPL_WITH_CGAL
  boost::optional< VoronoiSlabGenerator> voronoiSlab;
#endif
};

SCHEMER_MAP(StructureGeneratorSchema, StructureGenerator)
{
  element("builder", &StructureGenerator::builder);
#ifdef SPL_WITH_CGAL
  element("voronoiSlab", &StructureGenerator::voronoiSlab);
#endif
}

} // namespace builder

// STRUCTURE COMPARATORS //////////////////////////

struct SortedDistance
{
  double tolerance;
  bool volumeAgnostic;
  bool usePrimitive;
};

SCHEMER_MAP(SortedDistanceSchema, SortedDistance)
{
  element("tol", &SortedDistance::tolerance)->defaultValue(
      utility::SortedDistanceComparator::DEFAULT_TOLERANCE);
  element("volAgnostic", &SortedDistance::volumeAgnostic)->defaultValue(false);
  element("usePrimitive", &SortedDistance::usePrimitive)->defaultValue(false);
}

struct Comparator
{
  boost::optional< SortedDistance> sortedDist;
};

SCHEMER_MAP(ComparatorSchema, Comparator)
{
  element("sortedDist", &Comparator::sortedDist);
}

}
}

#endif /* SPL_WITH_YAML */
#endif /* SSLIB_YAML_SCHEMA_H */

