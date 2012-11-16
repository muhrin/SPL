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
#include <boost/filesystem.hpp>

#include "SSLib.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
}

namespace sstbx {
namespace io {

class IStructureReader
{
public:

	virtual ~IStructureReader() {}

	/**
	/* Read a structure from disk.  Additional data may be read if avilable
	/* and will be written out by any writers that support it.
	/* The user can supply their own species database, however it is up to them
	/* to make sure that the implementation is thread safe if necessary.
	/**/
  virtual UniquePtr<common::Structure>::Type readStructure(
		const ::boost::filesystem::path &     filepath,
		const ::sstbx::common::AtomSpeciesDatabase & speciesDb) const = 0;

	virtual ::std::vector<std::string> getSupportedFileExtensions() const = 0;
};

}}

#endif /* I_STRUCTURE_READER_H */
