/*
 * StructureYamlGenerator.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/StructureYamlGenerator.h"

#include <algorithm>
#include <sstream>

#include <boost/foreach.hpp>

#include <armadillo>

#include "spl/common/Atom.h"
#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/Structure.h"
#include "spl/common/UnitCell.h"
#include "spl/io/IoFunctions.h"
#include "spl/utility/IndexingEnums.h"

// DEFINES /////////////////////////////////

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {

namespace structure_properties = common::structure_properties;

StructureYamlGenerator::StructureYamlGenerator(
    const AtomYamlFormatParser::AtomsFormat & format) :
    myAtomInfoParser(format)
{
}

Structure
StructureYamlGenerator::generateInfo(
    const spl::common::Structure & structure) const
{
  Structure str;

  // Name
  if(!structure.getName().empty())
    str.name = structure.getName();

  // Unit cell
  if(structure.getUnitCell())
    str.unitCell = generateUnitCell(*structure.getUnitCell());

  // Atoms
  for(size_t i = 0; i < structure.getNumAtoms(); ++i)
    str.atoms.push_back(generateAtom(structure.getAtom(i)));

  // Properties
  Properties props;
  extractProperties(structure, &props);
  if(!props.empty())
    str.properties = props;

  return str;
}

common::types::StructurePtr
StructureYamlGenerator::generateStructure(const Structure & info) const
{
  common::types::StructurePtr structure(new common::Structure());

  if(info.name)
    structure->setName(*info.name);

  if(info.unitCell)
  {
    BOOST_ASSERT(info.unitCell->abc.size() == 6);
    double abc[6];
    std::copy(info.unitCell->abc.begin(), info.unitCell->abc.end(), abc);
    structure->setUnitCell(common::UnitCell(abc));
  }

  BOOST_FOREACH(const Atom & atomInfo, info.atoms)
  {
    common::Atom & atom = structure->newAtom(atomInfo.species);
    atom.setPosition(atomInfo.pos.t());
    if(atomInfo.radius)
      atom.setRadius(*atomInfo.radius);
  }

  if(info.properties)
    injectProperties(*info.properties, structure.get());

  return structure;
}

Atom
StructureYamlGenerator::generateAtom(const spl::common::Atom & atom) const
{
  Atom atomInfo;
  atomInfo.species = atom.getSpecies();
  atomInfo.pos = atom.getPosition().t();
  if(atom.getRadius() != 0.0)
    atomInfo.radius = atom.getRadius();

  return atomInfo;
}

UnitCell
StructureYamlGenerator::generateUnitCell(const common::UnitCell & cell) const
{
  UnitCell unitCellInfo;
  unitCellInfo.abc.resize(6);
  for(size_t i = 0; i < 6; ++i)
    unitCellInfo.abc[i] = cell.getLatticeParams()[i];

  unitCellInfo.volume = cell.getVolume();

  return unitCellInfo;
}

void
StructureYamlGenerator::extractProperties(const common::Structure & structure,
    Properties * const properties) const
{
  BOOST_FOREACH(const StructureProperty & property, structure_properties::VISIBLE_PROPERTIES)
  {
    const boost::optional< std::string> value = structure.getVisibleProperty(
        property);

    if(value)
      (*properties)[property.getName()] = *value;
  }
}

void
StructureYamlGenerator::injectProperties(const Properties & properties,
    common::Structure * const structure) const
{
  BOOST_FOREACH(Properties::const_reference p, properties)
  {
    common::Structure::VisibleProperty * const property =
        structure_properties::VISIBLE_PROPERTIES.getProperty(p.first);

    if(property)
      structure->setVisibleProperty(*property, p.second);
  }
}

}
}
