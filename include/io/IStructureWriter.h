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

#include <boost/filesystem.hpp>

#include "io/ResourceLocator.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
}

namespace sstbx {
namespace io {

class IStructureWriter
{
public:

	virtual ~IStructureWriter() {}

	/**
	/* Write a structure out to disk.  Additional data may be supplied if avilable
	/* and will be written out by any writers that support it.
	/* The user can supply their own species database, however it is up to them
	/* to make sure that the implementation is thread safe if necessary.
	/**/
	virtual void writeStructure(
		::sstbx::common::Structure & str,
		const ResourceLocator & locator,
		const ::sstbx::common::AtomSpeciesDatabase & speciesDb) const = 0;

	virtual ::std::vector<std::string> getSupportedFileExtensions() const = 0;
};

}}

#endif /* I_STRUCTURE_WRITER_H */
