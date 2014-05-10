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
#include "spl/common/Types.h"
#include "spl/common/StructureProperties.h"
#include "spl/io/AtomYamlFormatParser.h"
#include "spl/io/StructureSchema.h"

namespace spl {

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
  typedef AtomYamlFormatParser::FormatEntry FormatEntry;
  typedef AtomYamlFormatParser::AtomsFormat AtomsFormat;

  StructureYamlGenerator()
  {
  }
  StructureYamlGenerator(const AtomsFormat & atomsFormat);

  Structure
  generateInfo(const common::Structure & structure) const;

  common::types::StructurePtr
  generateStructure(const Structure & structureInfo) const;

private:
  typedef std::map< std::string, std::string> Properties;
  typedef utility::NamedProperty< utility::HeterogeneousMap> StructureProperty;

  Atom
  generateAtom(const common::Atom & atom) const;
  UnitCell
  generateUnitCell(const common::UnitCell & cell) const;
  void
  extractProperties(const common::Structure & structure,
      Properties * const properties) const;
  void
  injectProperties(const Properties & properties,
      common::Structure * const structure) const;

  AtomYamlFormatParser myAtomInfoParser;

};

}
}

#endif /* STRUCTURE_YAML_GENERATOR_H */
