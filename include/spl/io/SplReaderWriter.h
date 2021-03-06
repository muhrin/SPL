/*
 * SplReaderWriter.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SPL_READER_WRITER_H
#define SPL_READER_WRITER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/io/IStructureReader.h"
#include "spl/io/IStructureWriter.h"

#include <vector>

#include "spl/io/StructureYamlGenerator.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace io {

class SplReaderWriter : public IStructureWriter, public IStructureReader
{
public:
  static const unsigned int DIGITS_AFTER_DECIMAL;
  static const std::string DEFAULT_EXTENSION;

  // Write a structure out to disk.
  // The user can supply their own species database, however it is up to them
  // to make sure that the implementation is thread safe if necessary.
  virtual void
  writeStructure(spl::common::Structure & str,
      const ResourceLocator & locator) const;

  // From IStructureReader //
  virtual spl::common::types::StructurePtr
  readStructure(const ResourceLocator & resourceLocator) const;

  virtual size_t
  readStructures(StructuresContainer & outStructures,
      const ResourceLocator & resourceLocator) const;

  virtual std::vector< std::string>
  getSupportedFileExtensions() const;
  // End from IStructureReader //

  virtual bool
  multiStructureSupport() const;
};

}
}

#endif /* SPL_READER_WRITER_H */
