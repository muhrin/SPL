/*
 * Atom.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_H
#define ATOM_H

// INCLUDES ///////////////////////////////////
#include "common/AtomSpeciesId.h"

#include <armadillo>

namespace sstbx {
namespace common {

// FORWARD DECLARES ///////////////////////////
class Structure;

class Atom
{
public:

  Structure & getStructure();
  const Structure & getStructure() const;

  const ::arma::vec3 & getPosition() const;
  void setPosition(const ::arma::vec3 & pos);
  void setPosition(const double x, const double y, const double z);

  double getRadius() const;
  void setRadius(const double radius);

	const AtomSpeciesId::Value getSpecies() const;

  size_t getIndex() const;

private:

  Atom(const AtomSpeciesId::Value species, Structure & structure, const size_t index);

  /**
  /* Special constructor for use by Structure that copies everything an
  /* atom except the structure and index within the structure which are
  /* provided by the caller.
  /*
  /* Make sure to update this if any changes are made to Atom!
  /**/
  Atom(const Atom & toCopy, Structure & structure, const size_t index);

  void setIndex(const size_t index);

  Structure &           myStructure;

  /** The index of this atom in the structure. */
  size_t                myIndex;

  AtomSpeciesId::Value	mySpecies;
  ::arma::vec3          myPosition;
	double                myRadius;

  friend class Structure;
};

}} // Close the namespace

#endif /* ATOM_H */
