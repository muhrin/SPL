/*
 * Atom.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_H
#define ATOM_H

// INCLUDES ///////////////////////////////////
#include "AtomSpeciesId.h"

#include <armadillo>

// FORWARD DECLARES ///////////////////////////
namespace sstbx { namespace common {
	class AtomGroup;
	class StructureTreeEvent;
}}

namespace sstbx { namespace common {

class Atom {
public:

	typedef arma::Col<double>::fixed<3> Vec3;
	typedef arma::Mat<double>			Mat;

	friend class AtomGroup;

	Atom(const AtomSpeciesId species);

	const Vec3 & getPosition() const;
	void setPosition(const Vec3 & pos);

	const AtomSpeciesId getSpecies() const;

private:

	AtomGroup * getParent() const;
	void setParent(AtomGroup * const parent);

	void eventFired(const StructureTreeEvent & evt);

	AtomGroup * myParent;

	AtomSpeciesId	mySpecies;

	Vec3 position;

	double myRadius;
};

}} // Close the namespace

#endif /* ATOM_H */
