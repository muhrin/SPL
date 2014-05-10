/*
 * AtomSpciesDatabase.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/common/AtomSpeciesDatabase.h"

#include "spl/common/AtomSpeciesId.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace common {

AtomSpeciesDatabase::AtomSpeciesDatabase()
{
  setAll("H", "Hydrogen", 0.25);
  setAll("He", "Helium", 0.28);
  setAll("Li", "Lithium", 1.45);
  setAll("Be", "Beryllium", 1.05);
  setAll("B", "Boron", 0.85);
  setAll("C", "Carbon", 0.7);
  setAll("N", "Nitrogen", 0.65);
  setAll("O", "Oxygen", 0.6);
  setAll("F", "Flourine", 0.5);
  setAll("Ne", "Neon", 0.58);
  setAll("Na", "Sodium", 1.8);
  setAll("Mg", "Magnesium", 1.6);
  setAll("Al", "Aluminium", 1.43);
  setAll("Si", "Silicon", 1.11);
  setAll("P", "Phosphorus", 1.07);
  setAll("Cl", "Chlorine", 1.0);
  setAll("Ti", "Titanium", 1.47);
  setAll("Mn", "Manganese", 1.27);
  setAll("Fe", "Iron", 1.26);
  setAll("Ni", "Nickel", 1.24);
  setAll("As", "Arsenic", 1.19);
  setAll("Sr", "Strontium", 2.15);
  setAll("In", "Indium", 1.67);
}

const ::std::string *
AtomSpeciesDatabase::getName(const AtomSpeciesId::Value id) const
{
  SpeciesString::const_iterator it = myNames.find(id);
  if(it == myNames.end())
    return NULL;
  return &it->second;
}

void
AtomSpeciesDatabase::setName(const AtomSpeciesId::Value id,
    const ::std::string & name)
{
  myNames[id] = name;
}

::boost::optional< double>
AtomSpeciesDatabase::getRadius(const AtomSpeciesId::Value id) const
{
  ::boost::optional< double> rad;
  SpeciesDouble::const_iterator it = myRadii.find(id);
  if(it != myRadii.end())
    rad.reset(it->second);
  return rad;
}

void
AtomSpeciesDatabase::setRadius(const AtomSpeciesId::Value id,
    const double radius)
{
  myRadii[id] = radius;
}

OptionalDouble
AtomSpeciesDatabase::getSpeciesPairDistance(const SpeciesPair & pair) const
{
  // Try getting the most specific from the species pairs
  const SpeciesPairDistances::const_iterator it = mySpeciesPairDistances.find(
      pair);
  if(it != mySpeciesPairDistances.end())
    return it->second;

  // Try r1 + r2
  const SpeciesDouble::const_iterator radiiIt1 = myRadii.find(pair.first());
  if(radiiIt1 != myRadii.end())
  {
    const SpeciesDouble::const_iterator radiiIt2 = myRadii.find(pair.second());
    if(radiiIt2 != myRadii.end())
      return radiiIt1->second + radiiIt2->second;
  }

  return OptionalDouble();
}

void
AtomSpeciesDatabase::setSpeciesPairDistance(const SpeciesPair & pair,
    const double dist)
{
  mySpeciesPairDistances[pair] = dist;
}

void
AtomSpeciesDatabase::setAll(AtomSpeciesId::Value id, const ::std::string & name,
    const double radius)
{
  setName(id, name);
  setRadius(id, radius);
}

}
}
