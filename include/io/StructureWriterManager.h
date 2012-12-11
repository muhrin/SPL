/*
 * StructureWriterManager.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_WRITER_MANAGER_H
#define STRUCTURE_WRITER_MANAGER_H

// INCLUDES /////////////////////////////////////////////

#include <map>

#include <boost/filesystem.hpp>

#include "common/Types.h"
#include "io/IStructureReader.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
namespace io {
class IStructureWriter;
class ResourceLocator;
}
}


namespace sstbx {
namespace io {

class StructureWriterManager
{
public:

	void registerWriter(IStructureWriter & writer);
	void deregisterWriter(IStructureWriter & writer);

  void registerReader(IStructureReader & reader);
  void deregisterReader(IStructureReader & reader);

	
	bool writeStructure(
		common::Structure & str,
		const ResourceLocator & path,
    const common::AtomSpeciesDatabase & atomSpeciesDb) const;

  common::types::StructurePtr readStructure(
    const ResourceLocator & locator,
    const common::AtomSpeciesDatabase & speciesDb) const;

  size_t readStructures(
    io::StructuresContainer & outStructures,
    const ResourceLocator & locator,
    const common::AtomSpeciesDatabase & speciesDb) const;

protected:

  typedef ::std::map< ::std::string, IStructureWriter *> WritersMap;
  typedef ::std::map< ::std::string, IStructureReader *> ReadersMap;

  bool getExtension(::std::string & ext, const ResourceLocator & locator) const;

	WritersMap myWriters;
  ReadersMap myReaders;
};

}}

#endif /* STRUCTURE_WRITER_MANAGER_H */
