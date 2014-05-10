/*
 * IStructureWriter.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_WRITER_H
#define I_STRUCTURE_WRITER_H

// INCLUDES /////////////////////////////////////////////

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "spl/io/ResourceLocator.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class Structure;
}
}

namespace spl {
namespace io {

class IStructureWriter
{
public:

  virtual ~IStructureWriter() {}

  // Write a structure out to disk.
  virtual void writeStructure(
    ::spl::common::Structure & str,
    const ResourceLocator & locator
  ) const = 0;

  virtual ::std::vector<std::string> getSupportedFileExtensions() const = 0;

  // Does this writer support writing multiple structures to a single file.
  virtual bool multiStructureSupport() const = 0;
};

}
}

#endif /* I_STRUCTURE_WRITER_H */
