/*
 * IPotential.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_POTENTIAL_H
#define I_POTENTIAL_H

// INCLUDES /////////////////////////////////////////////
#include "StandardData.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx
{
	namespace common
	{
		class Structure;
	}
	namespace potential
	{
		class IPotentialInfo;
	}
}

namespace sstbx { namespace potential {

template <class DataType = StandardData<> >
class IPotential
{
public:

	virtual ~IPotential() {}

	virtual void evalPotential(
		const sstbx::common::Structure & structure,
		DataType & potentialData) const = 0;


	virtual DataType * generatePotentialData(sstbx::common::Structure & structure) const = 0;
};

}}

#endif /* I_POTENTIAL_H */
