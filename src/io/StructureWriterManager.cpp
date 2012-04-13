/*
 * StructureWriterManager.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "StructureWriterManager.h"

#include "io/IStructureWriter.h"

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////


namespace sstbx { namespace io {

void StructureWriterManager::registerWriter(sstbx::io::IStructureWriter &writer)
{
	using ::std::string;

	BOOST_FOREACH(string ext, writer.getSupportedFileExtensions())
	{
		myExtensionsMap.insert(ExtensionsMapPair(ext, &writer));
	}
}

void StructureWriterManager::deregisterWriter(sstbx::io::IStructureWriter &writer)
{
	using ::std::string;

	for(ExtensionsMap::iterator it = myExtensionsMap.begin(), end = myExtensionsMap.end();
		it != end; /*increment done in loop*/)
	{
		if(it->second == &writer)
		{
			myExtensionsMap.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void StructureWriterManager::writeStructure(
	const ::sstbx::common::Structure & str,
	const ::boost::filesystem::path & path,
	const AdditionalData * const data) const
{
	// TODO: Add status return value to this method
	using ::std::string;

	// Get the extension
	if(!path.has_filename())
	{
		return /*invalid path*/;
	}

	::boost::filesystem::path extPath = path.extension();

	if(extPath.empty())
	{
		return /*no extension*/;
	}

	string ext = extPath.string(); // Returns e.g. '.txt'
	ext.erase(0,1); // Erase the dot from the extensions

	ExtensionsMap::const_iterator it = myExtensionsMap.find(ext);

	if(it == myExtensionsMap.end())
	{
		return /*unknown extension*/;
	}

	// Finally pass it on the the correct writer
	it->second->writeStructure(str, path, data);
}

}}