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

#include <boost/filesystem.hpp>

#include <map>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
namespace io {
class IStructureWriter;
}
}


namespace sstbx {
namespace io {

class StructureWriterManager
{
public:

	void registerWriter(IStructureWriter & writer);
	void deregisterWriter(IStructureWriter & writer);

	
	bool writeStructure(
		common::Structure & str,
		const ::boost::filesystem::path & path,
    const common::AtomSpeciesDatabase & atomSpeciesDb) const;


protected:

	typedef ::std::map<std::string, IStructureWriter *> ExtensionsMap;

	ExtensionsMap	myExtensionsMap;
};

}}

#endif /* STRUCTURE_WRITER_MANAGER_H */
