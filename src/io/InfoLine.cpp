/*
 * InfoLine.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: muhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/InfoLine.h"

#include <boost/tokenizer.hpp>

#include "spl/common/Structure.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {
namespace detail {
const std::string EMPTY = "na";
}

namespace properties = common::structure_properties;

typedef boost::tokenizer< boost::char_separator< char> > Tok;

// Set up our tokenizer to split around space and tab
static const boost::char_separator< char> SEP(" \t");

InfoLine::InfoLine()
{
}

InfoLine::InfoLine(const common::Structure & str)
{
  set(str);
}

void
InfoLine::set(const common::Structure & str)
{
  if(!str.getName().empty())
    name = str.getName();

  // Presssure
  {
    const double * const sPressure = str.getProperty(
        properties::general::PRESSURE);
    if(sPressure)
      pressure = *sPressure;
  }

  // Volume
  {
    const common::UnitCell * const cell = str.getUnitCell();
    if(cell)
      volume = cell->getVolume();
  }

  // Enthalpy
  {
    const double * sEnthalpy = str.getProperty(properties::general::ENTHALPY);
    if(!sEnthalpy)
      sEnthalpy = str.getProperty(properties::general::ENERGY_INTERNAL);
    if(sEnthalpy)
      enthalpy = *sEnthalpy;
  }

  // Num atoms
  numAtoms = str.getNumAtoms();

  // Space group
  {
    const std::string * const sg = str.getProperty(
        properties::general::SPACEGROUP_SYMBOL);
    if(sg)
      spaceGroup = *sg;
  }

  // Times found
  {
    const unsigned int * const xFound = str.getProperty(
        properties::searching::TIMES_FOUND);
    if(xFound)
      timesFound = *xFound;
  }
}

void
InfoLine::populate(common::Structure * const structure) const
{
  if(name)
    structure->setName(*name);

  structure->setProperty(properties::general::PRESSURE, pressure);
  structure->setProperty(properties::general::ENTHALPY, enthalpy);
  structure->setProperty(properties::general::SPACEGROUP_SYMBOL, spaceGroup);
  structure->setProperty(properties::searching::TIMES_FOUND, timesFound);
}

std::ostream &
operator <<(std::ostream & os, const InfoLine & line)
{
  // Name
  if(line.name)
    os << *line.name;
  else
    os << detail::EMPTY;

  // Presssure
  os << " ";
  if(line.pressure)
    os << *line.pressure;
  else
    os << "0.0";

  // Volume
  os << " ";
  if(line.volume)
    os << line.volume;
  else
    os << "0.0";

  // Enthalpy
  os << " ";
  if(line.enthalpy)
    os << *line.enthalpy;
  else
    os << "0.0";

  // Spin density
  os << " ";
  if(line.spinDensity)
    os << *line.spinDensity;
  else
    os << "0.0";

  os << " ";
  if(line.integratedSpinDensity)
    os << *line.integratedSpinDensity;
  else
    os << "0.0";

  // Num atoms
  if(line.numAtoms)
    os << *line.numAtoms;
  else
    os << "0";

  // Space group
  os << " (";
  if(line.spaceGroup)
    os << *line.spaceGroup;
  else
    os << "P1";

  // Times found
  os << ") n - ";
  if(line.timesFound)
    os << *line.timesFound;
  else
    os << "1";

  return os;
}

std::istream &
operator >>(std::istream &in, InfoLine & line)
{
  std::string titleLine;
  std::getline(in, titleLine);
  if(titleLine.empty())
    return in;

  const Tok tok(titleLine, SEP);
  Tok::const_iterator it = tok.begin();
  const Tok::const_iterator end = tok.end();

  // 1 - Name
  if(it == end)
    return in;
  line.name = *it;

  // 2 - Pressure
  if(++it == end)
    return in;
  detail::setValue(*it, &line.pressure);

  // 3 - Volume
  if(++it == end)
    return in;
  detail::setValue(*it, &line.volume);

  // 4 - Enthalpy
  if(++it == end)
    return in;
  detail::setValue(*it, &line.enthalpy);

  // 5 - spin density
  if(++it == end)
    return in;
  detail::setValue(*it, &line.spinDensity);

  // 6 - integrated spin density
  if(++it == end)
    return in;
  detail::setValue(*it, &line.integratedSpinDensity);

  // 7 - space group OR num atoms(in new format ONLY)
  if(++it == end)
    return in;

  if(!it->empty() && (*it)[0] != '(')
  {
    detail::setValue(*it, &line.numAtoms); // New style format
    if(++it == end)
      return in;
  }

  {
    std::string iucSymbol = *it;
    if(!iucSymbol.empty() && iucSymbol[0] == '(')
      iucSymbol.erase(0, 1);
    if(!iucSymbol.empty() && iucSymbol[iucSymbol.size() - 1] == ')')
      iucSymbol.erase(iucSymbol.size() - 1, 1);
    detail::setValue(iucSymbol, &line.spaceGroup);
  }

  // 8 = 'n'
  if(++it == end)
    return in;
  // 9 = '-'
  if(++it == end)
    return in;
  // 10 = times found
  if(++it == end)
    return in;
  detail::setValue(*it, &line.timesFound);

  return in;
}

}
}
