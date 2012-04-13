/*
 * IGeomOptimiser.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_GEOM_OPTIMISER_H
#define I_GEOM_OPTIMISER_H

// INCLUDES /////////////////////////////////////////////


// DEFINES //////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx
{
	namespace common
	{
		class Structure;
	}
	namespace potential
	{
		template <typename FloatType>
		struct StandardData;
	}
}

namespace sstbx { namespace potential {

class IGeomOptimiser
{
public:

	virtual ~IGeomOptimiser() {}

	virtual bool optimise(::sstbx::common::Structure & structure) const = 0;

	virtual bool optimise(
		::sstbx::common::Structure & structure,
		StandardData<double> * & data) const = 0;
};

}}

#endif /* I_GEOM_OPTIMISER_H */
