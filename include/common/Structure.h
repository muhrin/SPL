/*
 * Structure.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_H
#define STRUCTURE_H

// INCLUDES ///////////////////////////////////////////////
#include "../SSLib.h"
#include "AtomGroup.h"

#include <vector>
#include <map>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx { namespace common {
	enum AtomSpeciesId;

	template <typename FloatType>
	class AbstractFmidCell;
}}

namespace sstbx { namespace common {

	class Structure : public AtomGroup {
public:

	Structure(AbstractFmidCell<double> * const cell = NULL);
	virtual ~Structure();

	const std::string & getName() const;
	void setName(const std::string & name);

	// UNIT CELL /////////////////////////////////////////

	AbstractFmidCell<double> * getUnitCell();
	const AbstractFmidCell<double> * getUnitCell() const;

	/**
	/* Set the unit cell to be used by the structure.  The Structure takes ownership
	/* and will delete the cell when necessary.
	/**/
	void setUnitCell(AbstractFmidCell<double> * const cell);

	// ATOMS ///////////////////////////////////////////////

	/** Get the total number of atoms in the structure */
	virtual size_t getNumAtomsDescendent() const;

	virtual void getAtomPositionsDescendent(Mat & posMtx, const size_t i = 0) const;

	virtual void setAtomPositionsDescendent(const Mat & posMtx);

private:

	virtual void eventFired(const StructureTreeEvent & evt);
	
	/** Methods to respond to child group messages */
	void childAdded(AtomGroup & childGroup);
	void childRemoved(AtomGroup & childGroup);

	/** Methods to respond atom messages */
	void atomAdded(Atom & atom);
	void atomRemoved(Atom & atom);

	/** The unit cell for this crystal structure. */
	AbstractFmidCell<double> * myCell;

	/** The name of this structure, set by calling code */
	std::string		myName;

	/** The total number of atoms in the structure */
	size_t			myNumAtomsDescendent;

	/**
	/* Flag to indicate whether the structure has changed since
	/* the last time that all atom positions were requested
	/**/
	mutable bool			myAtomPositionsCurrent;

	mutable Mat				myAtomPositionsBuffer;
};

}}

#endif /* STRUCTURE_H */
