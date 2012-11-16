/*
 * StructureWriterManager.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "io/StructureWriterManager.h"

#include "io/IStructureWriter.h"

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace io {

void StructureWriterManager::registerWriter(sstbx::io::IStructureWriter &writer)
{
	using ::std::string;

	BOOST_FOREACH(string ext, writer.getSupportedFileExtensions())
	{
    myExtensionsMap.insert(ExtensionsMap::value_type(ext, &writer));
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

bool StructureWriterManager::writeStructure(
	const ::sstbx::common::Structure & str,
	const ::boost::filesystem::path & path,
  const common::AtomSpeciesDatabase & atomSpeciesDb) const
{
	// TODO: Add status return value to this method
	using ::std::string;

	// Get the extension
	if(!path.has_filename())
	{
		return false; /*invalid path*/
	}

	::boost::filesystem::path extPath = path.extension();

	if(extPath.empty())
	{
		return false; /*no extension*/
	}

	string ext = extPath.string(); // Returns e.g. '.txt'
	ext.erase(0,1); // Erase the dot from the extensions

	ExtensionsMap::const_iterator it = myExtensionsMap.find(ext);

	if(it == myExtensionsMap.end())
	{
		return false; /*unknown extension*/
	}

	// Finally pass it on the the correct writer
	it->second->writeStructure(str, path, atomSpeciesDb);

  // TODO: The write may have failed so provide better and accurate feedback!
  return true;
}

}}
