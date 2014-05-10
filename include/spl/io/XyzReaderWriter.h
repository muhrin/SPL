/*
 * XyzReaderWriter.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef XYZ_READER_WRITER_H
#define XYZ_READER_WRITER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/io/IStructureReader.h"
#include "spl/io/IStructureWriter.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace io {

class XyzReaderWriter : public virtual IStructureWriter,
    public virtual IStructureReader
{
public:

  static const unsigned int DIGITS_AFTER_DECIMAL;

  // Write a structure out to disk.
  // The user can supply their own species database, however it is up to them
  // to make sure that the implementation is thread safe if necessary.
  virtual void
  writeStructure(spl::common::Structure & str,
      const ResourceLocator & locator) const;

  virtual common::types::StructurePtr readStructure(
    const ResourceLocator & resourceLocator
  ) const;

  // Read structure(s) from disk.
  virtual size_t readStructures(
    StructuresContainer & outStructures,
    const ResourceLocator & resourceLocator
  ) const;

  virtual std::vector< std::string>
  getSupportedFileExtensions() const;

  // Does this reader support reading multiple structures from a single file.
  virtual bool
  multiStructureSupport() const;

private:
  void
  readAtoms(std::istream * const is, common::Structure * const structure) const;
};

}
}

#endif /* XYZ_READER_WRITER_H */
