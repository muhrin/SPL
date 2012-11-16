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
#include "SSLib.h"

#ifdef SSLIB_USE_YAML

#include <map>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/utility.hpp>

#include <yaml-cpp/yaml.h>

// Local includes
#include "build_cell/IStructureGenerator.h"
#include "build_cell/StructureDescription.h"
#include "build_cell/Types.h"
#include "factory/FactoryError.h"
#include "factory/SsLibYamlKeywords.h"
#include "factory/YamlCommon.h"
#include "io/IStructureWriter.h"
#include "potential/IGeomOptimiser.h"
#include "potential/IPotential.h"
#include "potential/SimplePairPotential.h"
#include "potential/TpsdGeomOptimiser.h"
#include "utility/IStructureComparator.h"



// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace build_cell {
class AtomsDescription;
}
namespace common {
class AtomSpeciesDatabase;
}
namespace utility {
class UniqueStructureSet;
}

namespace factory {

class SsLibFactoryYaml : ::boost::noncopyable
{
public:

  typedef UniquePtr<utility::UniqueStructureSet>::Type UniqueStructureSetPtr;

  enum ErrorCode
  {
    UNKNOWN,
    BAD_TAG,
    UNRECOGNISED_KEYWORD,
    REQUIRED_KEYWORD_MISSING,
    MALFORMED_VALUE,
    SEQUENCE_LENGTH_INVALID
  };

  SsLibFactoryYaml(common::AtomSpeciesDatabase & atomSpeciesDb);

  common::StructurePtr                           createStructure(const YAML::Node & structureNode) const;
  common::UnitCellPtr                            createUnitCell(const YAML::Node & cellNode) const;
  build_cell::UnitCellBlueprintPtr               createRandomCellGenerator(const YAML::Node & desc);
  build_cell::StructureDescriptionPtr            createStructureDescription(const YAML::Node & desc);
  potential::IPotential *                        createPotential(const YAML::Node & desc);
  potential::IGeomOptimiser *                    createGeometryOptimiser(const YAML::Node & desc);
  utility::IStructureComparator *                createStructureComparator(const YAML::Node & node);
  UniqueStructureSetPtr                          createStructureSet(const YAML::Node & desc);
  io::IStructureWriter *                         createStructureWriter(const YAML::Node & node);


private:

  typedef ::boost::optional<double> OptionalDouble;
  typedef ::std::pair<common::AtomSpeciesId::Value, unsigned int> AtomSpeciesCount;
  typedef ::boost::optional<AtomSpeciesCount> OptionalAtomSpeciesCount;
  typedef ::boost::optional<YAML::Node> OptionalNode;
  typedef ::boost::optional<const YAML::Node> OptionalConstNode;
  typedef ::boost::optional< ::std::string> OptionalString;
  typedef ::std::set< ::std::string> AtomsInfoSet;
  typedef ::std::pair< ::std::string, OptionalString> AtomsInfoEntry;
  typedef ::std::vector<AtomsInfoEntry> AtomsInfoFormat;

  template <typename T>
  struct OptionalMinMax : private ::boost::noncopyable
  {
    typedef ::std::pair< ::boost::optional<T>, ::boost::optional<T> > Type;
  private:
    OptionalMinMax() {}
  };

  struct MinMaxRequire
  {
    enum Value
    {
      NEITHER = 0x00,   // 000
      MIN     = 0x01,   // 001
      MAX     = 0x02,   // 010
      BOTH    = 0x03,   // 011
    };
  };

  build_cell::AtomsDescriptionPtr                createAtomsDescription(const YAML::Node & desc, OptionalDouble atomsRadii = OptionalDouble()) const;
  build_cell::StructureConstraintDescription *   createStructureConstraintDescription(const YAML::Node & descNode) const;

  void checkKeyword(const sslib_yaml_keywords::KwTyp & kw, const YAML::Node & node) const;

  OptionalNode getChildNode(const YAML::Node & parent, const ::std::string & childNodeName) const;

  OptionalAtomSpeciesCount parseAtomTypeString(const ::std::string & atomSpecString) const;

  // AtomsInfoFormat parsing ////////////////////
  void parseAtomsInfoFormat(const YAML::Node & infoNode, const AtomsInfoSet & allowedValues, AtomsInfoFormat & formatOut) const;
  AtomsInfoEntry parseAtomsInfoFormatEntry(const YAML::Node & entryNode, const AtomsInfoSet & allowedValues) const;
  // End AtomsInfoFormat parsing ////////////////

  template <typename T>
  ::boost::optional<T> getNodeChildValueAs(const YAML::Node & node, const ::std::string & childName, const bool required = false) const;

  template <typename T>
  typename OptionalMinMax<T>::Type getMinMax(const YAML::Node & parentNode, const MinMaxRequire::Value requiredFlag = MinMaxRequire::NEITHER) const;

  common::AtomSpeciesDatabase &   myAtomSpeciesDb;

  AtomsInfoFormat myDefaultStructureAtomsInfoFormat;
  AtomsInfoSet myStructureAtomInfoSet;

  ::boost::ptr_vector< ::sstbx::io::IStructureWriter>                          myStructureWriters;
  ::boost::ptr_vector< ::sstbx::potential::IGeomOptimiser>                     myOptimisers;
  ::boost::ptr_vector< ::sstbx::potential::IPotential>                         myPotentials;
  ::boost::ptr_vector< ::sstbx::utility::IStructureComparator >                myStructureComparators;
  ::boost::ptr_vector< ::sstbx::build_cell::StructureDescription>              myStructureDescriptions;


};

// TYPEDEFS /////////////////
typedef ::boost::error_info<struct TagErrorType, SsLibFactoryYaml::ErrorCode>    ErrorType;
typedef ::boost::error_info<struct TagNodeName, ::std::string>                   NodeName;
typedef ::boost::error_info<struct TagValue, ::std::string>                      ProblemValue;

template <typename T>
::boost::optional<T>
SsLibFactoryYaml::getNodeChildValueAs(const YAML::Node & node, const ::std::string & childName, const bool required) const
{
  ::boost::optional<T> childValue;
  if(node[childName])
  {
    const YAML::Node & childNode = node[childName];
    if(!childNode.IsScalar())
    {
      BOOST_THROW_EXCEPTION(FactoryError() <<
        ErrorType(MALFORMED_VALUE) <<
        NodeName(childName) <<
        ProblemValue(childNode.as< ::std::string>()));
    }
    try
    {
      childValue.reset(childNode.as<T>());
    }
    catch(YAML::TypedBadConversion<double> e)
    {
      BOOST_THROW_EXCEPTION(FactoryError() <<
        ErrorType(MALFORMED_VALUE) <<
        NodeName(childName) <<
        ProblemValue(childNode.as< ::std::string>()));
    }
  }
  else if(required)
  {
    BOOST_THROW_EXCEPTION(FactoryError() <<
      ErrorType(REQUIRED_KEYWORD_MISSING) <<
      NodeName(childName));
  }
  return childValue;
}

template <typename T>
typename SsLibFactoryYaml::OptionalMinMax<T>::Type
SsLibFactoryYaml::getMinMax(const YAML::Node & parentNode, const SsLibFactoryYaml::MinMaxRequire::Value requiredFlag) const
{
  typename OptionalMinMax<T>::Type minMax;

  minMax.first  = getNodeChildValueAs<T>(parentNode, sslib_yaml_keywords::MIN, (bool)(requiredFlag & MinMaxRequire::MIN));
  minMax.second = getNodeChildValueAs<T>(parentNode, sslib_yaml_keywords::MAX, (bool)(requiredFlag & MinMaxRequire::MAX));

  return minMax;
}

}
}


#endif /* SSLIB_USE_YAML */
#endif /* SS_LIB_FACTORY_YAML_H */

