/*
 * Atom.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "spl/common/Atom.h"

#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"
#include "spl/common/Structure.h"

namespace spl {
namespace common {

Atom::Atom(const AtomSpeciesId::Value & species) :
    myIndex(-1), mySpecies(species), myRadius(-1.0)
{
}

Atom::Atom(const AtomSpeciesId::Value & species, const size_t index) :
    myIndex(index), mySpecies(species), myRadius(-1.0)
{
}

Atom::Atom(const Atom & toCopy) :
    myIndex(-1), mySpecies(toCopy.getSpecies()), myPosition(
        toCopy.getPosition()), myRadius(toCopy.getRadius())
{
}

Atom::~Atom()
{
  sendDestroyedMsg();
}

Atom &
Atom::operator ==(const Atom & rhs)
{
  mySpecies = rhs.mySpecies;
  myPosition = rhs.myPosition;
  myRadius = rhs.myRadius;

  return *this;
}

const ::arma::vec3 &
Atom::getPosition() const
{
  return myPosition;
}

void
Atom::setPosition(const ::arma::vec3 & pos)
{
  myPosition = pos;
  sendMovedMsg();
}

void
Atom::setPosition(const double x, const double y, const double z)
{
  myPosition(0) = x;
  myPosition(1) = y;
  myPosition(2) = z;
  sendMovedMsg();
}

void
Atom::moveBy(const ::arma::vec3 & dr)
{
  myPosition += dr;
  sendMovedMsg();
}

double
Atom::getRadius() const
{
  return myRadius;
}

void
Atom::setRadius(const double radius)
{
  myRadius = radius;
}

const AtomSpeciesId::Value &
Atom::getSpecies() const
{
  return mySpecies;
}

size_t
Atom::getIndex() const
{
  return myIndex;
}

void
Atom::setIndex(const size_t index)
{
  myIndex = index;
}

void
Atom::addListener(Listener * const listener)
{
  myListeners.insert(listener);
}

bool
Atom::removeListener(Listener * const listener)
{
  return myListeners.erase(listener);
}

void
Atom::sendMovedMsg()
{
  BOOST_FOREACH(Listener * const l, myListeners)
  {
    l->onAtomMoved(this);
  }
}

void
Atom::sendDestroyedMsg()
{
  BOOST_FOREACH(Listener * const l, myListeners)
  {
    l->onAtomDestroyed(this);
  }
}

}
}
