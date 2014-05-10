/*
 * AtomGroup.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_GROUP_H
#define ATOM_GROUP_H

// INCLUDES ///////////////////////////////////
#include "spl/SSLib.h"

#include <vector>

#include <boost/iterator/indirect_iterator.hpp>

#include <armadillo>

namespace spl {
namespace common {

// FORWARD DECLARES ///////////////////////////
class Atom;

class AtomGroup
{
  typedef ::std::vector<Atom *> Atoms;
public:

  typedef ::boost::indirect_iterator<Atoms::iterator> iterator;
  typedef ::boost::indirect_iterator<Atoms::const_iterator> const_iterator;

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  void pushBack(Atom & atom);
  iterator erase(iterator pos);

  ::arma::vec3 getCentre() const;
  void moveBy(const ::arma::vec3 & dr);

  double getVolume() const;

private:

  Atoms myAtoms;

};

}
}

#endif /* ATOM_GROUP_H */
