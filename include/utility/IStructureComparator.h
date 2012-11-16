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

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class Structure;
}
namespace utility {
class IBufferedComparator;
}
}

namespace sstbx
{
namespace utility
{

class IStructureComparator
{
public:

	virtual ~IStructureComparator() {}

	virtual double compareStructures(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const = 0;

	virtual bool areSimilar(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const = 0;

  virtual ::boost::shared_ptr<IBufferedComparator> generateBuffered() const = 0;
};

}
}

#endif /* I_STRUCTURE_COMPARATOR_H */
