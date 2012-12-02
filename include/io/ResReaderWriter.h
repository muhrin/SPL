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
#include "io/IStructureReader.h"
#include "io/IStructureWriter.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class AtomSpeciesDatabase;
}
}


namespace sstbx {
namespace io {

class ResReaderWriter :
  public IStructureWriter,
  public IStructureReader
{
public:

  static const unsigned int DIGITS_AFTER_DECIMAL;

	/**
	/* Write a structure out to disk.  Additional data may be supplied if avilable
	/* and will be written out by any writers that support it.
	/* The user can supply their own species database, however it is up to them
	/* to make sure that the implementation is thread safe if necessary.
	/**/
	virtual void writeStructure(
		::sstbx::common::Structure & str,
		const ::boost::filesystem::path & filepath,
		const common::AtomSpeciesDatabase & speciesDb) const;

  // From IStructureReader //

  virtual UniquePtr<common::Structure>::Type readStructure(
		const ::boost::filesystem::path &     filepath,
		const common::AtomSpeciesDatabase & speciesDb) const;

	virtual ::std::vector<std::string> getSupportedFileExtensions() const;

  // End from IStructureReader //

};

}}

#endif /* RES_READER_WRITER_H */
