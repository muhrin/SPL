/*
 * ResReaderWriter.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef RES_READER_WRITER_H
#define RES_READER_WRITER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/io/IStructureReader.h"
#include "spl/io/IStructureWriter.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace io {

class ResReaderWriter : public virtual IStructureWriter,
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

  // From IStructureReader //

  virtual spl::common::types::StructurePtr
  readStructure(const ResourceLocator & resourceLocator) const;

  virtual size_t
  readStructures(StructuresContainer & outStructures,
      const ResourceLocator & resourceLocator) const;

  virtual std::vector< std::string>
  getSupportedFileExtensions() const;

  // End from IStructureReader //

  // Does this reader support reading multiple structures from a single file.
  virtual bool
  multiStructureSupport() const;

private:
  bool
  parseCell(common::Structure & structure, const std::string & cellLine) const;
  bool
  parseAtoms(common::Structure & structure, std::istream & inStream,
      const std::string & sfacLine) const;

  void
  writeTitle(std::ostream & os, const common::Structure & structure) const;
};

}
}

#endif /* RES_READER_WRITER_H */
