/*
 * StructureSchema.h
 *
 *
 *  Created on: Jan 10, 2013
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_SCHEMA_H
#define STRUCTURE_SCHEMA_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include <boost/optional.hpp>

#include <armadillo>

#include <schemer/Schemer.h>

#include "spl/factory/SsLibYamlSchema.h"

namespace spl {
namespace io {

struct Atom
{
  std::string species;
  arma::rowvec3 pos;
  boost::optional< double> radius;
};

SCHEMER_MAP(AtomSchema, Atom)
{
  element("spec", &Atom::species);
  element< factory::Rowvec3>("pos", &Atom::pos);
  element("radius", &Atom::radius);
}

struct UnitCell
{
  std::vector< double> abc;
  boost::optional< double> volume;
};

SCHEMER_MAP(UnitCellSchema, UnitCell)
{
  element("abc", &UnitCell::abc);
  element("volume", &UnitCell::volume);
}

struct Structure
{
  boost::optional< std::string> name;
  std::vector< Atom> atoms;
  boost::optional< UnitCell> unitCell;
  boost::optional< std::map< std::string, std::string> > properties;
};

SCHEMER_MAP(StructureSchema, Structure)
{
  element("name", &Structure::name);
  element< schemer::List< AtomSchema> >("atoms", &Structure::atoms);
  element("unitCell", &Structure::unitCell);
  element("properties", &Structure::properties);
}

}
}

#endif /* STRUCTURE_SCHEMA_H */
