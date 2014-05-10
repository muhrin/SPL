/*
 * Atom.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_H
#define ATOM_H

// INCLUDES ///////////////////////////////////
#include "spl/SSLib.h"

#include <set>

#include "spl/common/AtomSpeciesId.h"

#include <armadillo>

namespace spl {
namespace common {

// FORWARD DECLARES ///////////////////////////

class Atom
{
public:
  class Listener
  {
  public:
    virtual void
    onAtomMoved(Atom * const atom) = 0;
    virtual void
    onAtomDestroyed(Atom * const atom) = 0;
  };

  Atom(const AtomSpeciesId::Value & species);
  Atom(const AtomSpeciesId::Value & species, const size_t index);
  // Make sure to update this if any changes are made to Atom!
  Atom(const Atom & toCopy);
  ~Atom();

  Atom &
  operator ==(const Atom & rhs);

  const ::arma::vec3 &
  getPosition() const;
  void
  setPosition(const ::arma::vec3 & pos);
  void
  setPosition(const double x, const double y, const double z);
  void
  moveBy(const ::arma::vec3 & dr);

  double
  getRadius() const;
  void
  setRadius(const double radius);

  const AtomSpeciesId::Value &
  getSpecies() const;

  size_t
  getIndex() const;
  void
  setIndex(const size_t index);

  void
  addListener(Listener * const listener);
  bool
  removeListener(Listener * const listener);

private:
  typedef ::std::set<Listener *> Listeners;

  void
  sendMovedMsg();
  void
  sendDestroyedMsg();

  /** The index of this atom in the structure. */
  size_t myIndex;

  AtomSpeciesId::Value mySpecies;
  ::arma::vec3 myPosition;
  double myRadius;

  Listeners myListeners;
};

}
}

#endif /* ATOM_H */
