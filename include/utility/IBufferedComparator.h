/*
 * IBufferedComparator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_BUFFERED_COMPARATOR_H
#define I_BUFFERED_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////

#include <limits>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx{
namespace common {
class Structure;
}
namespace utility {
class IStructureComparator;
}
}

namespace sstbx {
namespace utility {

class IBufferedComparator
{
public:

  virtual ~IBufferedComparator() {}

	virtual double compareStructures(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) = 0;

	virtual bool areSimilar(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) = 0;

  virtual bool hasComparisonDataFor(const ::sstbx::common::Structure & str) = 0;
  virtual bool releaseComparisonDataFor(const ::sstbx::common::Structure & str) = 0;

  virtual const IStructureComparator & getComparator() const = 0;
};

}
}

#endif /* I_BUFFERED_COMPARATOR_H */
