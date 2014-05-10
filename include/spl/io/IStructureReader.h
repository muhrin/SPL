/*
 * ISructureReader.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_READER_H
#define I_STRUCTURE_READER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <vector>

#include <boost/ptr_container/ptr_vector.hpp>

#include "spl/common/Types.h"
#include "spl/io/ResourceLocator.h"

namespace spl {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
}

namespace io {

typedef ::boost::ptr_vector<common::Structure> StructuresContainer;

class IStructureReader
{
public:

  virtual ~IStructureReader() {}

  virtual common::types::StructurePtr readStructure(
    const ResourceLocator & resourceLocator
  ) const = 0;

  // Read structure(s) from disk.
  virtual size_t readStructures(
    StructuresContainer & outStructures,
    const ResourceLocator & resourceLocator
  ) const = 0;

  virtual ::std::vector<std::string> getSupportedFileExtensions() const = 0;

  // Does this reader support reading multiple structures from a single file.
  virtual bool multiStructureSupport() const = 0;
};

}
}

#endif /* I_STRUCTURE_READER_H */
