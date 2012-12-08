/*
 * StructureYamlGenerator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_YAML_GENERATOR_H
#define STRUCTURE_YAML_GENERATOR_H

// INCLUDES /////////////////////////////////////////////
#include <yaml-cpp/yaml.h>

#include "common/Types.h"
#include "common/StructureProperties.h"

namespace sstbx {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Atom;
class AtomSpeciesDatabase;
class Structure;
}

namespace io {

class StructureYamlGenerator
{
public:
  StructureYamlGenerator(const common::AtomSpeciesDatabase & speciesDb);

  YAML::Node generateNode(const common::Structure & structure) const;

  common::types::StructurePtr generateStructure(const YAML::Node & node) const;

private:

  typedef utility::NamedProperty<utility::HeterogeneousMap> StructureProperty;

  YAML::Node generateNode(const common::Atom & atom) const;
  bool addProperty(
    YAML::Node & propertiesNode,
    const common::Structure & structure,
    const StructureProperty & property) const;

  void praseProperties(
    common::Structure & structure,
    const YAML::Node & propertiesNode) const;

  const common::AtomSpeciesDatabase & mySpeciesDb;

};

}
}

#endif /* STRUCTURE_YAML_GENERATOR_H */
