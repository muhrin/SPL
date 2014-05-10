/*
 * AtomGroup.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "spl/common/AtomGroup.h"

#include <boost/foreach.hpp>

#include "spl/common/Atom.h"
#include "spl/common/Constants.h"

namespace spl {
namespace common {

AtomGroup::iterator AtomGroup::begin()
{
  return iterator(myAtoms.begin());
}

AtomGroup::const_iterator AtomGroup::begin() const
{
  return const_iterator(myAtoms.begin());
}

AtomGroup::iterator AtomGroup::end()
{
  return iterator(myAtoms.end());
}

AtomGroup::const_iterator AtomGroup::end() const
{
  return const_iterator(myAtoms.end());
}

void AtomGroup::pushBack(Atom & atom)
{
  myAtoms.push_back(&atom);
}

AtomGroup::iterator AtomGroup::erase(iterator pos)
{
  return iterator(myAtoms.erase(pos.base()));
}

::arma::vec3 AtomGroup::getCentre() const
{
  ::arma::vec3 centre;
  centre.zeros();
  BOOST_FOREACH(const Atom * const atom, myAtoms)
  {
    centre += atom->getPosition();
  }
  return centre / static_cast<double>(myAtoms.size());
}

void AtomGroup::moveBy(const ::arma::vec3 & dr)
{
  BOOST_FOREACH(Atom * const atom, myAtoms)
  {
    atom->moveBy(dr);
  }
}

double AtomGroup::getVolume() const
{
  double radius, sumRadiiCubed = 0.0;
  BOOST_FOREACH(const Atom * const atom, myAtoms)
  {
    radius = atom->getRadius();
    sumRadiiCubed += radius * radius * radius;
  }
  return (4.0 / 3.0) * constants::PI * sumRadiiCubed;
}

}
}
