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
namespace sstbx
{
	namespace common
	{
		class Structure;
	}
	namespace io
	{
		struct AdditionalData;
		class IStructureWriter;
	}
}


namespace sstbx { namespace io {

class StructureWriterManager
{
public:

	void registerWriter(IStructureWriter & writer);
	void deregisterWriter(IStructureWriter & writer);

	
	void writeStructure(
		const ::sstbx::common::Structure & str,
		const ::boost::filesystem::path & path,
		const AdditionalData * const data = NULL) const;


protected:

	typedef ::std::map<std::string, IStructureWriter *> ExtensionsMap;
	typedef ::std::pair<std::string, IStructureWriter *> ExtensionsMapPair;

	ExtensionsMap	myExtensionsMap;
};

}}

#endif /* STRUCTURE_WRITER_MANAGER_H */
