/*
 * AtomSpciesDatabase.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "common/AtomSpeciesDatabase.h"

#include "common/AtomSpeciesId.h"

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace common {


AtomSpeciesDatabase::AtomSpeciesDatabase()
{
  setAll(AtomSpeciesId::H, "H", "Hydrogen");
  setAll(AtomSpeciesId::LI, "Li", "Lithium");
  setAll(AtomSpeciesId::BE, "Be", "Beryllium");
  setAll(AtomSpeciesId::B, "B", "Boron");
  setAll(AtomSpeciesId::C, "C", "Carbon");
	setAll(AtomSpeciesId::NA, "Na", "Sodium");
	setAll(AtomSpeciesId::CL, "Cl", "Chlorine");
}

const ::std::string * AtomSpeciesDatabase::getName(const AtomSpeciesId::Value id) const
{
	SpeciesString::const_iterator it = myNames.find(id);
	if(it == myNames.end())
		return NULL;
	return &it->second;
}

void AtomSpeciesDatabase::setName(const AtomSpeciesId::Value id, const ::std::string & name)
{
	myNames[id] = name;
}

const ::std::string * AtomSpeciesDatabase::getSymbol(const AtomSpeciesId::Value id) const
{
	SpeciesString::const_iterator it = mySymbols.find(id);
	if(it == mySymbols.end())
		return NULL;
	return &it->second;
}

void AtomSpeciesDatabase::setSymbol(const AtomSpeciesId::Value id, const ::std::string & symbol)
{
	mySymbols[id] = symbol;
}

const AtomSpeciesId::Value AtomSpeciesDatabase::getIdFromSymbol(const std::string & symbol) const
{
  AtomSpeciesId::Value id = AtomSpeciesId::DUMMY;
  for(SpeciesString::const_iterator it = mySymbols.begin(), end = mySymbols.end();
    it != end; ++it)
  {
    if(it->second == symbol)
    {
      id = it->first;
      break;
    }
  }
  return id;
}

::boost::optional<double> AtomSpeciesDatabase::getRadius(const AtomSpeciesId::Value id) const
{
  ::boost::optional<double> rad;
	SpeciesDouble::const_iterator it = myRadii.find(id);
	if(it != myRadii.end())
    rad.reset(it->second);
	return rad;
}

void AtomSpeciesDatabase::setRadius(const AtomSpeciesId::Value id, const double radius)
{
  myRadii[id] = radius;
}

void AtomSpeciesDatabase::setAll(
  AtomSpeciesId::Value id,
	const ::std::string & symbol,
	const ::std::string & name)
{
	setSymbol(id, symbol);
	setName(id, name);
}


}
}
