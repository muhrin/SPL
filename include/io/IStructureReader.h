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

#include <boost/ptr_container/ptr_vector.hpp>

#include "common/Types.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
}

namespace sstbx {
namespace io {

typedef ::boost::ptr_vector<common::Structure> StructuresContainer;

class IStructureReader
{
public:

	virtual ~IStructureReader() {}

  virtual ::sstbx::common::types::StructurePtr readStructure(
		const ::boost::filesystem::path &     filepath,
		const ::sstbx::common::AtomSpeciesDatabase & speciesDb) const = 0;

	/**
	/* Read structure(s) from disk.
	/* The user can supply their own species database, however it is up to them
	/* to make sure that the implementation is thread safe if necessary.
	/**/
  virtual size_t readStructures(
    StructuresContainer & outStructures,
		const ::boost::filesystem::path &     filepath,
		const ::sstbx::common::AtomSpeciesDatabase & speciesDb) const = 0;

	virtual ::std::vector<std::string> getSupportedFileExtensions() const = 0;
};

}}

#endif /* I_STRUCTURE_READER_H */
