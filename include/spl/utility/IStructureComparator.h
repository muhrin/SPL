/*
 * IStructureComparator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_COMPARATOR_H
#define I_STRUCTURE_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////

#include <boost/shared_ptr.hpp>

#include "spl/utility/UtilityFwd.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class Structure;
}
namespace utility {
class IBufferedComparator;
}
}

namespace spl {
namespace utility {

class IStructureComparator
{
public:

	virtual ~IStructureComparator() {}

	virtual double compareStructures(
		const spl::common::Structure & str1,
		const spl::common::Structure & str2) const = 0;

	virtual bool areSimilar(
		const spl::common::Structure & str1,
		const spl::common::Structure & str2) const = 0;

  virtual ::boost::shared_ptr<IBufferedComparator> generateBuffered() const = 0;
};

}
}

#endif /* I_STRUCTURE_COMPARATOR_H */
