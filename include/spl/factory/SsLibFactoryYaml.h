/*
 * SsLibFactoryYaml.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SS_LIB_FACTORY_YAML_H
#define SS_LIB_FACTORY_YAML_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <map>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/utility.hpp>

#include "spl/build_cell/BuildCellFwd.h"
#include "spl/build_cell/StructureBuilder.h"
#include "spl/build_cell/StructureGenerator.h"
#include "spl/build_cell/VoronoiSlabGenerator.h"
#include "spl/factory/FactoryFwd.h"
#include "spl/factory/GenShapeFactory.h"
#include "spl/factory/SsLibYamlSchema.h"
#include "spl/io/AtomFormatParser.h"
#include "spl/io/IStructureWriter.h"
#include "spl/potential/Types.h"
#include "spl/utility/HeterogeneousMapKey.h"
#include "spl/utility/IStructureComparator.h"
#include "spl/utility/UniqueStructureSet.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace build_cell {
class VoronoiSlabGenerator;
}
namespace common {
class AtomSpeciesDatabase;
}
namespace potential {
class OptimisationSettings;
}

namespace factory {

class Factory : boost::noncopyable
{
public:
  typedef UniquePtr< potential::GeomOptimiser>::Type GeomOptimiserPtr;
  typedef UniquePtr< utility::UniqueStructureSet< > >::Type UniqueStructureSetPtr;

  Factory();

  build_cell::AtomsDescriptionPtr
  createAtomsDescription(const builder::SimpleAtomsDataEntry & options,
      const io::AtomFormatParser< builder::SimpleAtomsData> & parser) const;

  build_cell::AtomsGroupPtr
  createAtomsGroup(const builder::AtomsGroup & options,
      io::AtomFormatParser< builder::SimpleAtomsData> & parser) const;

  GeomOptimiserPtr
  createGeometryOptimiser(const Optimiser & options) const;

  potential::OptimisationSettings
  createOptimisationSettings(const OptimiserSettings & options) const;

  potential::IPotentialPtr
  createPotential(const Potential & options) const;

  build_cell::RandomUnitCellPtr
  createRandomCellGenerator(const builder::UnitCellBuilder & options) const;

  build_cell::StructureBuilderPtr
  createStructureBuilder(const builder::Builder & options) const;

  utility::IStructureComparatorPtr
  createStructureComparator(const Comparator & options) const;

  build_cell::IStructureGeneratorPtr
  createStructureGenerator(const builder::StructureGenerator & options) const;
#ifdef SPL_WITH_CGAL
  UniquePtr< build_cell::VoronoiSlabGenerator>::Type
  createVoronoiSlabGenerator(const builder::VoronoiSlabGenerator & options) const;
#endif // SPL_WITH_CGAL

  const GenShapeFactory &
  getShapeFactory() const;

private:
#ifdef SPL_WITH_CGAL
  UniquePtr<build_cell::VoronoiSlabGenerator::SlabRegion::Basis>::Type
  createVoronoiSlabRegionBasis(const builder::VoronoiSlabRegionBasis & options) const;
#endif // SPL_WITH_CGAL

  const GenShapeFactory myShapeFactory;
};

}
}

#endif /* SS_LIB_FACTORY_YAML_H */

