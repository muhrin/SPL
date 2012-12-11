/*
 * StructureYamlGenerator.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "io/StructureYamlGenerator.h"

#include <sstream>

#include <boost/foreach.hpp>

#include <armadillo>

#include "common/Atom.h"
#include "common/AtomSpeciesDatabase.h"
#include "common/Structure.h"
#include "common/UnitCell.h"
#include "factory/SsLibYamlKeywords.h"
#include "io/IoFunctions.h"
#include "io/YamlTranscode.h"
#include "utility/IndexingEnums.h"

// DEFINES /////////////////////////////////


// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace io {

namespace kw = factory::sslib_yaml_keywords;
namespace structure_properties = common::structure_properties;

StructureYamlGenerator::StructureYamlGenerator(const common::AtomSpeciesDatabase & speciesDb):
mySpeciesDb(speciesDb)
{}

YAML::Node
StructureYamlGenerator::generateNode(const ::sstbx::common::Structure & structure) const
{
  using namespace utility::cell_params_enum;

  YAML::Node root;

  // Name
  if(!structure.getName().empty())
    root[kw::STRUCTURE__NAME] = structure.getName();
  
  // Unit cell
  const common::UnitCell * const cell = structure.getUnitCell();
  if(cell)
    root[kw::STRUCTURE__CELL] = *cell;

  // Atoms
  for(size_t i = 0 ; i < structure.getNumAtoms(); ++i)
    root[kw::STRUCTURE__ATOMS].push_back(generateNode(structure.getAtom(i)));

  // Properties
  BOOST_FOREACH(const StructureProperty & property, structure_properties::VISIBLE_PROPERTIES)
    addProperty(root[kw::STRUCTURE__PROPERTIES], structure, property);
  

  return root;
}

common::types::StructurePtr
StructureYamlGenerator::generateStructure(const YAML::Node & node) const
{
  common::types::StructurePtr structure(new common::Structure());

  bool valid = true;

  if(node[kw::STRUCTURE__NAME])
    structure->setName(node[kw::STRUCTURE__NAME].as< ::std::string>());
  
  if(node[kw::STRUCTURE__CELL])
  {
    common::types::UnitCellPtr cell(new common::UnitCell());
    *cell = node[kw::STRUCTURE__CELL].as<common::UnitCell>();
    structure->setUnitCell(cell);
  }

  if(node[kw::STRUCTURE__ATOMS] && node[kw::STRUCTURE__ATOMS].IsSequence())
  {
    common::AtomSpeciesId::Value species;
    ::arma::vec3 pos;
    BOOST_FOREACH(const YAML::Node & atomNode, node[kw::STRUCTURE__ATOMS])
    {
      if(atomNode[kw::STRUCTURE__ATOMS__SPEC])
      {
        species =
          mySpeciesDb.getIdFromSymbol(atomNode[kw::STRUCTURE__ATOMS__SPEC].as< ::std::string>());

        if(species != common::AtomSpeciesId::DUMMY)
        {
          common::Atom & atom = structure->newAtom(species);
          if(atomNode[kw::STRUCTURE__ATOMS__POS])
          {
            pos = atomNode[kw::STRUCTURE__ATOMS__POS].as< ::arma::vec3>();
            atom.setPosition(pos);
          }
        }
      }
    }
  }

  if(node[kw::STRUCTURE__PROPERTIES])
    praseProperties(*structure, node[kw::STRUCTURE__PROPERTIES]);

  return structure;
}

YAML::Node StructureYamlGenerator::generateNode(
  const ::sstbx::common::Atom & atom) const
{
  using namespace utility::cart_coords_enum;

  YAML::Node atomNode;

  // Species
  const ::std::string * const species = mySpeciesDb.getSymbol(atom.getSpecies());
  if(species)
    atomNode[kw::STRUCTURE__ATOMS__SPEC] = *species;

  // Position
  atomNode[kw::STRUCTURE__ATOMS__POS] = atom.getPosition();

  return atomNode;
}

bool StructureYamlGenerator::addProperty(
  YAML::Node propertiesNode,
  const common::Structure & structure,
  const StructureProperty & property) const
{
  ::boost::optional< ::std::string> value = structure.getVisibleProperty(property);

  if(!value)
    return false;

  propertiesNode[property.getName()] = *value;

  return true;
}

void StructureYamlGenerator::praseProperties(
  common::Structure & structure,
  const YAML::Node & propertiesNode) const
{
  common::Structure::VisibleProperty * property;
  for(YAML::const_iterator it = propertiesNode.begin(), end = propertiesNode.end();
    it != end; ++it)
  {
    property =
      structure_properties::VISIBLE_PROPERTIES.getProperty(it->first.as< ::std::string>());

    if(property)
    {
      structure.setVisibleProperty(*property, it->second.as< ::std::string>());
    }
  }
}

}
}
