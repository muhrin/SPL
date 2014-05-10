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

#include "spl/utility/SharedHandle.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class Structure;
}
namespace utility {
class IStructureComparator;
}
}

namespace spl {
namespace utility {

class IBufferedComparator : protected SharedHandle< size_t>::Notifiee
{
protected:
  typedef size_t HandleId;
public:
  typedef SharedHandle< HandleId> ComparisonDataHandle;

  virtual
  ~IBufferedComparator()
  {
  }

  virtual double
  compareStructures(const ComparisonDataHandle & str1,
      const ComparisonDataHandle & str2) = 0;

  virtual bool
  areSimilar(const ComparisonDataHandle & str1,
      const ComparisonDataHandle & str2) = 0;

  virtual ComparisonDataHandle
  generateComparisonData(const spl::common::Structure & structure) = 0;
  virtual const IStructureComparator &
  getComparator() const = 0;

protected:
  virtual void
  handleReleased(const HandleId & id) = 0;

  friend class SharedHandle< HandleId> ;
};

}
}

#endif /* I_BUFFERED_COMPARATOR_H */
