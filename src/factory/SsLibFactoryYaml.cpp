/*
 * SsLibFactoryYaml.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "SSLib.h"

#ifdef SSLIB_USE_YAML

#include "factory/SsLibFactoryYaml.h"

#include <memory>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>

#include <yaml-cpp/yaml.h>

// Local includes
#include "build_cell/AtomsDescription.h"
#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/DefaultCrystalGenerator.h"
#include "build_cell/RandomUnitCell.h"
#include "build_cell/StructureConstraintDescription.h"
#include "common/AtomSpeciesDatabase.h"
#include "common/AtomSpeciesId.h"
#include "factory/FactoryError.h"
#include "io/ResReaderWriter.h"
#include "utility/IndexingEnums.h"
#include "utility/SortedDistanceComparator.h"
#include "utility/UniqueStructureSet.h"



// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace factory {

// namespace aliases
namespace ssbc  = ::sstbx::build_cell;
namespace ssc   = ::sstbx::common;
namespace ssio  = ::sstbx::io;
namespace ssp   = ::sstbx::potential;
namespace ssu   = ::sstbx::utility;

namespace kw = sslib_yaml_keywords;

// Boost Tokenizer stuff
typedef boost::tokenizer<boost::char_separator<char> > Tok;
const boost::char_separator<char> tokSep(" \t");

SsLibFactoryYaml::SsLibFactoryYaml(common::AtomSpeciesDatabase & atomSpeciesDb):
myAtomSpeciesDb(atomSpeciesDb)
{
  // All the possible options that can go in a structure atom info node
  myStructureAtomInfoSet.insert(kw::STRUCTURE__ATOMS__SPEC);
  myStructureAtomInfoSet.insert(kw::STRUCTURE__ATOMS__POS);

  myDefaultStructureAtomsInfoFormat.push_back(AtomsInfoEntry(kw::STRUCTURE__ATOMS__SPEC, OptionalString()));
  myDefaultStructureAtomsInfoFormat.push_back(AtomsInfoEntry(kw::STRUCTURE__ATOMS__POS, OptionalString()));
}

common::StructurePtr
SsLibFactoryYaml::createStructure(const YAML::Node & structureNode) const
{
  ::std::string sValue;

  common::StructurePtr structure(new common::Structure());

  // Unit cell //////////////////////////
  if(structureNode[kw::STRUCTURE__CELL])
  {
    structure->setUnitCell(createUnitCell(structureNode[kw::STRUCTURE__CELL]));
  }

  // Atoms ////////////////////////////

  // First get any atom defaults if they exist
  YAML::Node atomDefaults;
  if(structureNode[kw::STRUCTURE__ATOM_DEFAULTS])
  {
    atomDefaults = structureNode[kw::STRUCTURE__ATOM_DEFAULTS];
  }

  AtomsInfoFormat atomInfoFormat;
  if(structureNode[kw::STRUCTURE__ATOM_INFO_FORMAT])
  {
    const YAML::Node & formatNode = structureNode[kw::STRUCTURE__ATOM_INFO_FORMAT];

    parseAtomsInfoFormat(formatNode, myStructureAtomInfoSet, atomInfoFormat);
  }
  else
  {
    atomInfoFormat = myDefaultStructureAtomsInfoFormat;
  }

  if(structureNode[kw::STRUCTURE__ATOMS])
  {
    const YAML::Node & atomsNode = structureNode[kw::STRUCTURE__ATOMS];

    if(atomsNode.IsSequence())
    {
      common::AtomSpeciesId::Value species;
      ::arma::vec3 pos;

      BOOST_FOREACH(const YAML::Node & atomNode, atomsNode)
      {
        if(atomNode.IsSequence())
        {
          //for(size_t i = 0; i < atomInfoFormat; ++i)
          //{
          //  
          //}
        }


        // Get the species
        if(!atomNode[kw::STRUCTURE__ATOMS__SPEC])
        {
          BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(REQUIRED_KEYWORD_MISSING) << NodeName(kw::STRUCTURE__ATOMS__SPEC));
        }
        const YAML::Node & type = atomNode[kw::STRUCTURE__ATOMS__SPEC];

        species = myAtomSpeciesDb.getIdFromSymbol(type.as< ::std::string>());

        // TODO: Get the position

        structure->newAtom(species);
      }
    }
    else
    {
      BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
    }
  }

  return structure;
}

common::UnitCellPtr
SsLibFactoryYaml::createUnitCell(const YAML::Node & cellNode) const
{
  common::UnitCellPtr cell;

  if(cellNode[kw::STRUCTURE__CELL__ABC])
  {
    const YAML::Node & abcNode = cellNode[kw::STRUCTURE__CELL__ABC];


  }

  return cell;
}

ssbc::UnitCellBlueprintPtr
SsLibFactoryYaml::createRandomCellGenerator(const YAML::Node & node)
{
  using namespace utility::cell_params_enum;

  ssbc::RandomUnitCellPtr cell(new ssbc::RandomUnitCell());

  if(node[kw::RANDOM_CELL__PARAMS])
  {
    const YAML::Node & paramsNode = node[kw::RANDOM_CELL__PARAMS];

    if(paramsNode.IsSequence() && paramsNode.size() == 6)
    {
      double params[6];
      for(size_t i = A; i <= GAMMA; ++i)
      {
        try
        {
          params[i] = paramsNode[i].as<double>();
        }
        catch(YAML::TypedBadConversion<double> e)
        {
          BOOST_THROW_EXCEPTION(FactoryError() <<
            ErrorType(MALFORMED_VALUE) <<
            NodeName(kw::RANDOM_CELL__PARAMS) <<
            ProblemValue(paramsNode[i].as< ::std::string>()));
        }
        cell->setMin(i, params[i]);
        cell->setMax(i, params[i]);
      }
    }
    else
    {
      BOOST_THROW_EXCEPTION(FactoryError() <<
        ErrorType(SEQUENCE_LENGTH_INVALID) <<
        NodeName(kw::RANDOM_CELL__PARAMS));
    }
  }

  OptionalDouble dValue;

  dValue = getNodeChildValueAs<double>(node, kw::RANDOM_CELL__VOL);
  if(dValue)
  {
    cell->setTargetVolume(*dValue);
  }

  if(node[kw::RANDOM_CELL__ANGLES])
  {
    const OptionalMinMax<double>::Type minMax = getMinMax<double>(node[kw::RANDOM_CELL__ANGLES]);
    if(minMax.first)
    {
      cell->setMinAngles(*minMax.first);
    }
    if(minMax.second)
    {
      cell->setMaxAngles(*minMax.second);
    }
  }

  if(node[kw::RANDOM_CELL__LENGTHS])
  {
    const YAML::Node & lengths = node[kw::RANDOM_CELL__LENGTHS];
    const OptionalMinMax<double>::Type minMax = getMinMax<double>(lengths);
    if(minMax.first)
    {
      cell->setMinLengths(*minMax.first);
    }
    if(minMax.second)
    {
      cell->setMaxLengths(*minMax.second);
    }
    dValue = getNodeChildValueAs<double>(lengths, kw::RANDOM_CELL__LENGTHS__MAX_RATIO);
    if(dValue)
    {
      cell->setMaxLengthRatio(dValue);
    }
  }

  return (ssbc::UnitCellBlueprintPtr)cell;
}

ssbc::StructureDescriptionPtr
SsLibFactoryYaml::createStructureDescription(const YAML::Node & node)
{
  //// Make sure we have a structure description node
  //if(node.Scalar() != kw::STR_DESC)
  //{
  //  throw FactoryError() << ErrorType(BAD_TAG) << NodeName(node.Scalar());
  //}

  ssbc::StructureDescriptionPtr strDescription(new ssbc::StructureDescription());

  const OptionalDouble atomsRadii = getNodeChildValueAs<double>(node, kw::RANDOM_STRUCTURE__ATOM_RADII);

  // Atoms //
  if(node[kw::STR_DESC__ATOMS])
  {
    const YAML::Node & atomsNode = node[kw::STR_DESC__ATOMS];

    if(atomsNode.IsSequence())
    {
      BOOST_FOREACH(const YAML::Node & atomNode, atomsNode)
      {
        strDescription->addChild(createAtomsDescription(atomNode, atomsRadii));
      }
    }
    else
    {
      BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
    }
  }

  // Unit cell
  if(node[kw::RANDOM_CELL])
  {
    strDescription->setUnitCell((ssbc::ConstUnitCellBlueprintPtr)createRandomCellGenerator(node[kw::RANDOM_CELL]));
  }

  // Assign the pointer so the caller gets the object
  return strDescription;
}

ssp::IPotential * SsLibFactoryYaml::createPotential(const YAML::Node & node)
{
  //// Make sure we have a potential node
  //if(node.Scalar() != kw::POTENTIAL)
  //{
  //  throw FactoryError() << ErrorType(BAD_TAG) << NodeName(node.Scalar());
  //}

  ssp::IPotential * pot = NULL;

  // First check that we have the keywords that we require in this node
  checkKeyword(kw::TYPE, node);

  const ::std::string potType = node[kw::TYPE].as< ::std::string>();

  if(potType == kw::POTENTIAL__TYPE___PAIR_POT)
  {
    // Check all the required keywords are there for the pair potential
    checkKeyword(kw::POTENTIAL__PAIR_POT__E, node);
    checkKeyword(kw::POTENTIAL__PAIR_POT__S, node);
    checkKeyword(kw::POTENTIAL__PAIR_POT__CUT, node);
    checkKeyword(kw::POTENTIAL__PAIR_POT__B, node);
    checkKeyword(kw::POTENTIAL__PAIR_POT__POW, node);
    checkKeyword(kw::POTENTIAL__PAIR_POT__COMB, node);

    // Let's build it up one by one
    const ArmaTriangularMat epsilon  = node[kw::POTENTIAL__PAIR_POT__E].as<ArmaTriangularMat>();
    const ArmaTriangularMat sigma    = node[kw::POTENTIAL__PAIR_POT__S].as<ArmaTriangularMat>();
    double                  cutoff   = node[kw::POTENTIAL__PAIR_POT__CUT].as<double>();
    const ArmaTriangularMat beta     = node[kw::POTENTIAL__PAIR_POT__B].as<ArmaTriangularMat>();
    const ::arma::vec       pow      = node[kw::POTENTIAL__PAIR_POT__POW].as< ::arma::vec>();
    const ssp::SimplePairPotential::CombiningRule comb =
      node[kw::POTENTIAL__PAIR_POT__COMB].as<ssp::SimplePairPotential::CombiningRule>();

    const size_t numSpecies = epsilon.mat.n_rows;

    if((numSpecies != sigma.mat.n_rows) || (numSpecies != beta.mat.n_rows))
    {
      throw FactoryError() << ErrorType(MALFORMED_VALUE);
    }

    // TODO: HACK!! Fix this to read in the real values
    const ::std::vector<ssc::AtomSpeciesId> species;

    pot = new ssp::SimplePairPotential(
      myAtomSpeciesDb,
      numSpecies,
      species,
      epsilon.mat,
      sigma.mat,
      cutoff,
      beta.mat,
      pow(0),
      pow(1),
      comb
    );

    myPotentials.push_back(pot);
  }
  else
  {
    throw FactoryError() << ErrorType(UNRECOGNISED_KEYWORD) << ProblemValue(potType);
  }



  return pot;

}



ssp::IGeomOptimiser * SsLibFactoryYaml::createGeometryOptimiser(const YAML::Node & node)
{
  checkKeyword(kw::TYPE, node);

  ssp::IGeomOptimiser * opt = NULL;

  const ::std::string type = node[kw::TYPE].as< ::std::string>();

  if(type == kw::OPTIMISER__TYPE___TPSD)
  {
    checkKeyword(kw::POTENTIAL, node);

    ssp::IPotential * pp = createPotential(node[kw::POTENTIAL]);

    if(pp)
    {
      opt = new ssp::TpsdGeomOptimiser(*pp);
    }
  }

  myOptimisers.push_back(opt);

  return opt;
}

::sstbx::utility::IStructureComparator *
SsLibFactoryYaml::createStructureComparator(const YAML::Node & node)
{
  //// Make sure we have a structure set node
  //if(node.Scalar() != kw::STR_COMPARATOR)
  //{
  //  throw FactoryError() << ErrorType(BAD_TAG) << NodeName(node.Scalar());
  //}

  checkKeyword(kw::TYPE, node);

  const ::std::string type = node[kw::TYPE].as< ::std::string>();

  ssu::IStructureComparator * comparator = NULL;

  if(type == kw::STR_COMPARATOR__TYPE___SORTED_DIST)
  {
    comparator = new ssu::SortedDistanceComparator();
    myStructureComparators.push_back(comparator);
  }
  else
  {
    throw FactoryError() << ErrorType(UNRECOGNISED_KEYWORD);
  }

  return comparator;
}

SsLibFactoryYaml::UniqueStructureSetPtr
SsLibFactoryYaml::createStructureSet(const YAML::Node & node)
{
  //// Make sure we have a structure set node
  //if(node.Scalar() != kw::STR_SET)
  //{
  //  throw FactoryError() << ErrorType(BAD_TAG) << NodeName(node.Scalar());
  //}

  // First try to create the comparator
  checkKeyword(kw::STR_COMPARATOR, node);

  ssu::IStructureComparator * const comparator = createStructureComparator(node[kw::STR_COMPARATOR]);

  UniqueStructureSetPtr strSet;
  if(comparator)
  {
    strSet.reset(new ssu::UniqueStructureSet(*comparator));
  }
  
  return strSet;
}

::sstbx::io::IStructureWriter *
SsLibFactoryYaml::createStructureWriter(const YAML::Node & node)
{
  //// Make sure we have a structure writer tag
  //if(node.Scalar() != kw::STR_SET)
  //{
  //  throw FactoryError() << ErrorType(BAD_TAG) << NodeName(node.Scalar());
  //}

  // Check we have the required keywords
  checkKeyword(kw::TYPE, node);

  const ::std::string type = node[kw::TYPE].as< ::std::string>();

  ssio::IStructureWriter * writer = NULL;
  if(type == kw::STR_WRITER__TYPE___RES)
  {
    writer = new ssio::ResReaderWriter();
  }

  if(writer)
    myStructureWriters.push_back(writer);

  return writer;
}

void SsLibFactoryYaml::checkKeyword(
  const kw::KwTyp & kw,
  const YAML::Node & node) const
{
  if(!node[kw])
    throw FactoryError() << ErrorType(REQUIRED_KEYWORD_MISSING);
}

build_cell::AtomsDescriptionPtr
SsLibFactoryYaml::createAtomsDescription(const YAML::Node & descNode, OptionalDouble atomsRadii) const
{
  std::string sValue;
  unsigned int nAtoms = 1;
  ssc::AtomSpeciesId::Value  specId;

  if(descNode[kw::STRUCTURE__ATOMS__SPEC])
  {
    sValue = descNode[kw::STRUCTURE__ATOMS__SPEC].as< ::std::string>();
    OptionalAtomSpeciesCount atomSpeciesCount = parseAtomTypeString(sValue);
    if(atomSpeciesCount)
    {
      specId = atomSpeciesCount->first;
      nAtoms = atomSpeciesCount->second;
    }
  }

  build_cell::AtomsDescriptionPtr atomsDescription(new build_cell::AtomsDescription(specId, nAtoms));

  // If there is a default atoms radii first set that and then try to see if there
  // is a specific one for this atom
  if(atomsRadii)
    atomsDescription->setRadius(*atomsRadii);

  if(descNode[kw::RANDOM_STRUCTURE__ATOMS__RADIUS])
  {
    atomsDescription->setRadius(descNode[kw::RANDOM_STRUCTURE__ATOMS__RADIUS].as<double>());
  }


  return atomsDescription;
}


ssbc::StructureConstraintDescription *
SsLibFactoryYaml::createStructureConstraintDescription(const YAML::Node & descNode) const
{
  // TODO: No structure constraint
  return NULL;
}

SsLibFactoryYaml::OptionalNode
SsLibFactoryYaml::getChildNode(const YAML::Node & parent, const ::std::string & childNodeName) const
{
  OptionalNode childNode;

  if(parent[childNodeName])
  {
    childNode.reset(parent[childNodeName]);
  }

  return childNode;
}

SsLibFactoryYaml::OptionalAtomSpeciesCount
SsLibFactoryYaml::parseAtomTypeString(const ::std::string & atomSpecString) const
{
  OptionalAtomSpeciesCount atomSpeciesCount;

  const Tok tok(atomSpecString, tokSep);

  Tok::const_iterator it = tok.begin();

  AtomSpeciesCount type;
  bool successful = false;
  if(it != tok.end())
  {
    try
    {
      type.second = ::boost::lexical_cast<unsigned int>(*it);
      ++it;
    }
    catch(::boost::bad_lexical_cast)
    {}

    if(it != tok.end())
    {
      type.first = myAtomSpeciesDb.getIdFromSymbol(*it);
      if(type.first != common::AtomSpeciesId::DUMMY)
        successful = true;
    }
  }

  if(successful)
    atomSpeciesCount.reset(type);

  return atomSpeciesCount;
}

void SsLibFactoryYaml::parseAtomsInfoFormat(
  const YAML::Node & infoNode,
  const AtomsInfoSet & allowedValues,
  AtomsInfoFormat & formatOut) const
{
  if(infoNode.IsSequence())
  {
    BOOST_FOREACH(const YAML::Node & entryNode, infoNode)
    {
      formatOut.push_back(parseAtomsInfoFormatEntry(entryNode, allowedValues));
    }
  }
  else
  {
    BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
  }
}

SsLibFactoryYaml::AtomsInfoEntry
SsLibFactoryYaml::parseAtomsInfoFormatEntry(
  const YAML::Node & entryNode,
  const AtomsInfoSet & allowedValues) const
{
  AtomsInfoEntry entry;
  ::std::string entryName;
  ::std::string defaultValue;

  if(entryNode.IsScalar())
  {
    entryName = ::boost::trim_copy(entryNode.as< ::std::string>());

    // Check that the entry is one of the allowed values
    if(allowedValues.find(entryName) == allowedValues.end())
    {
      BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
    }

    entry.first = entryName;
  }
  else if(entryNode.IsMap())
  {
    if(entryNode.size() == 1)
    {
      const YAML::const_iterator it = entryNode.begin();

      entryName = ::boost::trim_copy(it->first.as< ::std::string>());

      // Check that the entry is one of the allowed values
      if(allowedValues.find(entryName) == allowedValues.end())
      {
        BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
      }

      defaultValue = ::boost::trim_copy(it->second.as< ::std::string>());

      // Add it into the format info with no default value
      entry.first = entryName;
      entry.second = defaultValue;
    }
    else
    {
      BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
    }
  }
  else
  {
    BOOST_THROW_EXCEPTION(FactoryError() << ErrorType(MALFORMED_VALUE));
  }

  return entry;
}

}
}


#endif /* SSLIB_USE_YAML */
