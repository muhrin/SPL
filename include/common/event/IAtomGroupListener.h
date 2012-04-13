/*
 * IAtomGroupListener.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_ATOM_GROUP_LISTENER_H
#define I_ATOM_GROUP_LISTENER_H

// INCLUDES ///////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////
class AtomGroup;

namespace sstbx { namespace common { namespace event {

class IAtomGroupListener {
public:

	virtual void childChange(
		AtomGroup * const source,
		AtomGroup * const oldVal,
		AtomGroup * const newVal) = 0;

	virtual void atomChange(
		AtomGroup * const source,
		Atom * const oldVal,
		Atom * const newVal) = 0;
};

}}}

#endif /* I_ATOM_GROUP_LISTENER_H */
