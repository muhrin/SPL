/*
 * IStructureSet.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_SET_H
#define I_STRUCTURE_SET_H

// INCLUDES /////////////////////////////////////////////
#include <map>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx { namespace common {
	class Structure;
}}

namespace sstbx { namespace utility {

class IStructureSet
{
public:

	virtual ~IStructureSet() {}

	virtual const std::pair<sstbx::common::Structure *, bool>
		insert(sstbx::common::Structure * const str) = 0;

	virtual void clear() = 0;
};

}}

#endif /* I_STRUCTURE_SET_H */
