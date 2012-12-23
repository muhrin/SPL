/*
 * StructureReadWriteManager.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "io/StructureReadWriteManager.h"

#include "common/Structure.h"
#include "io/IStructureReader.h"
#include "io/IStructureWriter.h"
#include "io/ResourceLocator.h"

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace io {

namespace fs = ::boost::filesystem;

void StructureReadWriteManager::insertWriter(WriterPtr writer)
{
  myWritersStore.push_back(writer.release());
  registerWriter(myWritersStore.back());
}

void StructureReadWriteManager::insertReader(UniquePtr<IStructureReader>::Type reader)
{
  myReadersStore.push_back(reader.release());
  registerReader(myReadersStore.back());
}

void StructureReadWriteManager::registerWriter(sstbx::io::IStructureWriter &writer)
{
  BOOST_FOREACH(const ::std::string & ext, writer.getSupportedFileExtensions())
	{
    myWriters.insert(WritersMap::value_type(ext, &writer));
	}
}

void StructureReadWriteManager::deregisterWriter(sstbx::io::IStructureWriter &writer)
{
	using ::std::string;

	for(WritersMap::iterator it = myWriters.begin(), end = myWriters.end();
		it != end; /*increment done in loop*/)
	{
		if(it->second == &writer)
			myWriters.erase(it++);
		else
			++it;
	}
}

void StructureReadWriteManager::registerReader(IStructureReader & reader)
{
  BOOST_FOREACH(const ::std::string & ext, reader.getSupportedFileExtensions())
	{
    myReaders.insert(ReadersMap::value_type(ext, &reader));
	}
}

void StructureReadWriteManager::deregisterReader(IStructureReader & reader)
{
	for(ReadersMap::iterator it = myReaders.begin(), end = myReaders.end();
		it != end; /*increment done in loop*/)
	{
		if(it->second == &reader)
			myReaders.erase(it++);
		else
			++it;
	}
}

bool StructureReadWriteManager::writeStructure(
	::sstbx::common::Structure & str,
	const ResourceLocator & locator,
  const common::AtomSpeciesDatabase & atomSpeciesDb) const
{
	// TODO: Add status return value to this method
  ::std::string ext;
  if(!getExtension(ext, locator))
    return false;

	const WritersMap::const_iterator it = myWriters.find(ext);

	if(it == myWriters.end())
		return false; /*unknown extension*/

	// Finally pass it on the the correct writer
	it->second->writeStructure(str, locator, atomSpeciesDb);

  // TODO: The write may have failed so provide better and accurate feedback!
  return true;
}

common::types::StructurePtr StructureReadWriteManager::readStructure(
  const ResourceLocator & locator,
  const common::AtomSpeciesDatabase & speciesDb) const
{
	// TODO: Add status return value to this method

  common::types::StructurePtr structure;

  ::std::string ext;
  if(!getExtension(ext, locator))
    return structure;

	const ReadersMap::const_iterator it = myReaders.find(ext);

	if(it == myReaders.end())
		return structure; /*unknown extension*/

	// Finally pass it on the the correct reader
  structure = it->second->readStructure(locator, speciesDb);

  // TODO: The write may have failed so provide better and accurate feedback!
  return structure;
}

size_t StructureReadWriteManager::readStructures(
  io::StructuresContainer & outStructures,
  const ResourceLocator & locator,
  const common::AtomSpeciesDatabase & speciesDb) const
{
  ::std::string ext;
  if(!getExtension(ext, locator))
    return 0;

	const ReadersMap::const_iterator it = myReaders.find(ext);

	if(it == myReaders.end())
		return 0; /*unknown extension*/

	// Finally pass it on the the correct reader
  return it->second->readStructures(outStructures, locator, speciesDb);
}

bool StructureReadWriteManager::getExtension(::std::string & ext, const ResourceLocator & locator) const
{
	// Get the extension
	if(!locator.path().has_filename())
		return false; /*invalid path*/

	const fs::path extPath = locator.path().extension();
	if(extPath.empty())
		return false; /*no extension*/

  ext = extPath.string(); // Returns e.g. '.txt'
	ext.erase(0,1); // Erase the dot from the extensions

  return true;
}

}
}
