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

#include "IStructureWriter.h"

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx
{
	namespace common
	{
		class AtomSpeciesDatabase;
	}
}


namespace sstbx { namespace io {

class ResReaderWriter : public IStructureWriter
{
public:

	/**
	/* Write a structure out to disk.  Additional data may be supplied if avilable
	/* and will be written out by any writers that support it.
	/* WARNING: This method may rely on an atom species database in which case the
	/* default database will be used which is NOT thread safe.
	/**/
	virtual void writeStructure(
		const ::sstbx::common::Structure & str,
		const ::boost::filesystem::path & filepath,
		const AdditionalData * const data = NULL) const;

	/**
	/* Write a structure out to disk.  Additional data may be supplied if avilable
	/* and will be written out by any writers that support it.
	/* The user can supply their own species database, however it is up to them
	/* to make sure that the implementation is thread safe if necessary.
	/**/
	virtual void writeStructure(
		const ::sstbx::common::Structure & str,
		const ::boost::filesystem::path & filepath,
		const ::sstbx::common::AtomSpeciesDatabase & speciesDb,
		const AdditionalData * const data = NULL) const;

	virtual ::std::vector<std::string> getSupportedFileExtensions() const;

};

}}

#endif /* RES_READER_WRITER_H */
