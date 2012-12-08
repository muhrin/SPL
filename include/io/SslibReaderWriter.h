/*
 * SslibReaderWriter.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_READER_WRITER_H
#define SSLIB_READER_WRITER_H

// INCLUDES /////////////////////////////////////////////
#include "io/IStructureReader.h"
#include "io/IStructureWriter.h"

#include <vector>

#include "io/StructureYamlGenerator.h"

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx {
namespace io {

class SslibReaderWriter :
  public IStructureWriter,
  public IStructureReader
{
public:

  static const unsigned int DIGITS_AFTER_DECIMAL;

	/**
	/* Write a structure out to disk.
	/* The user can supply their own species database, however it is up to them
	/* to make sure that the implementation is thread safe if necessary.
	/**/
	virtual void writeStructure(
		::sstbx::common::Structure & str,
		const ::boost::filesystem::path & filepath,
		const common::AtomSpeciesDatabase & speciesDb) const;

  // From IStructureReader //

  virtual ::sstbx::common::types::StructurePtr readStructure(
		const ::boost::filesystem::path &     filepath,
		const ::sstbx::common::AtomSpeciesDatabase & speciesDb) const;

  virtual size_t readStructures(
    StructuresContainer & outStructures,
		const ::boost::filesystem::path &     filepath,
		const common::AtomSpeciesDatabase & speciesDb) const;

	virtual ::std::vector<std::string> getSupportedFileExtensions() const;

  // End from IStructureReader //

private:

};

}}

#endif /* SSLIB_READER_WRITER_H */
