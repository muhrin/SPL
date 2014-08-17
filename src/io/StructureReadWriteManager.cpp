/*
 * StructureReadWriteManager.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/StructureReadWriteManager.h"

#include "spl/common/Structure.h"
#include "spl/io/BoostFilesystem.h"
#include "spl/io/CastepReader.h"
#include "spl/io/CellReaderWriter.h"
#include "spl/io/ResourceLocator.h"
#include "spl/io/ResReaderWriter.h"
#include "spl/io/StructureReadWriteManager.h"
#include "spl/io/SplReaderWriter.h"
#include "spl/io/XyzReaderWriter.h"

#include <boost/foreach.hpp>
#ifdef SSLIB_ENABLE_THREAD_AWARE
#  include <boost/thread/locks.hpp>
#endif

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {

namespace fs = boost::filesystem;
namespace properties = common::structure_properties;

StructureReadWriteManager::StructureReadWriteManager()
{
  // Add all spl readers/writers to the manager
  insert(makeUniquePtr(new CastepReader()));
  insert(makeUniquePtr(new CellReaderWriter()));
  insert(makeUniquePtr(new ResReaderWriter()));
  insert(makeUniquePtr(new SplReaderWriter()));
  insert(makeUniquePtr(new XyzReaderWriter()));
  setDefaultWriter(SplReaderWriter::DEFAULT_EXTENSION);
}

StructureReadWriteManager::WritersIterator
StructureReadWriteManager::beginWriters()
{
  return WritersIterator(myWriters.begin());
}

StructureReadWriteManager::WritersConstIterator
StructureReadWriteManager::beginWriters() const
{
  return WritersConstIterator(myWriters.begin());
}

StructureReadWriteManager::WritersIterator
StructureReadWriteManager::endWriters()
{
  return WritersIterator(myWriters.end());
}

StructureReadWriteManager::WritersConstIterator
StructureReadWriteManager::endWriters() const
{
  return WritersConstIterator(myWriters.end());
}

StructureReadWriteManager::WritersRange
StructureReadWriteManager::writers()
{
  return WritersRange(beginWriters(), endWriters());
}

StructureReadWriteManager::WritersConstRange
StructureReadWriteManager::writers() const
{
  return WritersConstRange(beginWriters(), endWriters());
}

size_t
StructureReadWriteManager::numWriters() const
{
  return myWriters.size();
}

StructureReadWriteManager::ReadersIterator
StructureReadWriteManager::beginReaders()
{
  return ReadersIterator(myReaders.begin());
}

StructureReadWriteManager::ReadersConstIterator
StructureReadWriteManager::beginReaders() const
{
  return ReadersConstIterator(myReaders.begin());
}

StructureReadWriteManager::ReadersIterator
StructureReadWriteManager::endReaders()
{
  return ReadersIterator(myReaders.end());
}

StructureReadWriteManager::ReadersConstIterator
StructureReadWriteManager::endReaders() const
{
  return ReadersConstIterator(myReaders.end());
}

StructureReadWriteManager::ReadersRange
StructureReadWriteManager::readers()
{
  return ReadersRange(beginReaders(), endReaders());
}

StructureReadWriteManager::ReadersConstRange
StructureReadWriteManager::readers() const
{
  return ReadersConstRange(beginReaders(), endReaders());
}

size_t
StructureReadWriteManager::numReaders() const
{
  return myReaders.size();
}

void
StructureReadWriteManager::insertWriter(WriterPtr writer)
{
  myWritersStore.push_back(writer.release());
  registerWriter(myWritersStore.back());
}

void
StructureReadWriteManager::insertReader(
    UniquePtr< IStructureReader>::Type reader)
{
  myReadersStore.push_back(reader.release());
  registerReader(myReadersStore.back());
}

void
StructureReadWriteManager::registerWriter(spl::io::IStructureWriter &writer)
{
  BOOST_FOREACH(const std::string & ext, writer.getSupportedFileExtensions())
  {
    myWriters.insert(WritersMap::value_type(ext, &writer));
  }
}

void
StructureReadWriteManager::deregisterWriter(spl::io::IStructureWriter &writer)
{
  using std::string;

  for(WritersMap::iterator it = myWriters.begin(), end = myWriters.end();
      it != end; /*increment done in loop*/)
  {
    if(it->second == &writer)
      myWriters.erase(it++);
    else
      ++it;
  }
}

void
StructureReadWriteManager::registerReader(IStructureReader & reader)
{
  BOOST_FOREACH(const std::string & ext, reader.getSupportedFileExtensions())
  {
    myReaders.insert(ReadersMap::value_type(ext, &reader));
  }
}

void
StructureReadWriteManager::deregisterReader(IStructureReader & reader)
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

bool
StructureReadWriteManager::writeStructure(spl::common::Structure & str,
    ResourceLocator locator) const
{
  // TODO: Add status return value to this method
  std::string ext;
  if(!getExtension(ext, locator))
  {
    // No extension: try default writer
    if(!myDefaultWriteExtension.empty())
    {
      ext = myDefaultWriteExtension;
      locator.setPath(locator.path().string() + "." + ext);
    }
    else
      return false; // don't know which output format to use
  }

  return writeStructure(str, locator, ext);
}

bool
StructureReadWriteManager::writeStructure(common::Structure & str,
    ResourceLocator locator, const std::string & fileType) const
{
  // TODO: Add status return value to this method
  const WritersMap::const_iterator it = myWriters.find(fileType);

  if(it == myWriters.end())
    return false; // unknown extension

  if(!it->second->multiStructureSupport())
    locator.clearId();

#ifdef SSLIB_ENABLE_THREAD_AWARE
  boost::lock_guard< boost::mutex> guard(myRwMutex);
#endif

  // Finally pass it on the the correct writer
  it->second->writeStructure(str, locator);

  postWrite(str, locator);

  // TODO: The write may have failed so provide better and accurate feedback!
  return true;
}

common::types::StructurePtr
StructureReadWriteManager::readStructure(const ResourceLocator & locator) const
{
  // TODO: Add status return value to this method

  common::types::StructurePtr structure;

  std::string ext;
  if(!getExtension(ext, locator))
    return structure;

  const ReadersMap::const_iterator it = myReaders.find(ext);

  if(it == myReaders.end())
    return structure; /*unknown extension*/

#ifdef SSLIB_ENABLE_THREAD_AWARE
  boost::lock_guard< boost::mutex> guard(myRwMutex);
#endif

  // Finally pass it on the the correct reader
  structure = it->second->readStructure(locator);

  if(structure.get())
    postRead(*structure, locator);

  // TODO: The write may have failed so provide better and accurate feedback!
  return structure;
}

size_t
StructureReadWriteManager::readStructures(StructuresContainer & outStructures,
    const ResourceLocator & locator, const int maxDepth) const
{
  if(!fs::exists(locator.path()))
    return 0;

  if(fs::is_regular_file(locator.path()))
  {
    std::string ext;
    if(!getExtension(ext, locator))
      return 0;

    const ReadersMap::const_iterator it = myReaders.find(ext);

    if(it == myReaders.end())
      return 0; /*unknown extension*/

#ifdef SSLIB_ENABLE_THREAD_AWARE
    boost::lock_guard< boost::mutex> guard(myRwMutex);
#endif

    // Finally pass it on the the correct reader
    StructuresContainer structures;
    const size_t numRead = it->second->readStructures(structures, locator);
    SSLIB_ASSERT(numRead == structures.size());

    // Set the path to where it was read from
    BOOST_FOREACH(common::Structure & structure, structures)
      postRead(structure, locator);

    outStructures.transfer(outStructures.end(), structures);

    return numRead;
  }
  else if(fs::is_directory(locator.path()))
    return doReadAllStructuresFromPath(outStructures, locator.path(), maxDepth);
  else
    return 0;
}

const IStructureWriter *
StructureReadWriteManager::getWriter(const std::string & ext) const
{
  const WritersMap::const_iterator it = myWriters.find(ext);

  if(it == myWriters.end())
    return NULL; // unknown extension

  return it->second;
}

bool
StructureReadWriteManager::setDefaultWriter(const std::string & extension)
{
  myDefaultWriteExtension = extension;

  const WritersMap::const_iterator it = myWriters.find(extension);

  if(it == myWriters.end())
    return false;

  return true;
}

const IStructureWriter *
StructureReadWriteManager::getDefaultWriter() const
{
  const WritersMap::const_iterator it = myWriters.find(myDefaultWriteExtension);

  if(it == myWriters.end())
    return NULL;

  return it->second;
}

bool
StructureReadWriteManager::getExtension(std::string & ext,
    const ResourceLocator & locator) const
{
  // Get the extension
  if(!locator.path().has_filename())
    return false; /*invalid path*/

  const fs::path extPath = locator.path().extension();
  if(extPath.empty())
    return false; /*no extension*/

  ext = extPath.string(); // Returns e.g. '.txt'
  ext.erase(0, 1); // Erase the dot from the extensions

  return true;
}

size_t
StructureReadWriteManager::doReadAllStructuresFromPath(
    StructuresContainer & outStructures, const boost::filesystem::path & path,
    const size_t maxDepth, const size_t currentDepth) const
{
  // Preconditions:
  // fs::exists(path)
  // fs::is_directory(path)
  if(currentDepth > maxDepth)
    return 0;

  size_t numRead = 0;

  BOOST_FOREACH(const fs::path & entry, std::make_pair(fs::directory_iterator(path), fs::directory_iterator()))
  {
    if(fs::is_regular_file(entry))
    {
      numRead += readStructures(outStructures, entry, maxDepth);
    }
    else if(currentDepth < maxDepth && fs::is_directory(entry))
    {
      numRead += doReadAllStructuresFromPath(outStructures, entry, maxDepth,
          currentDepth + 1);
    }
  }

  return numRead;
}

void
StructureReadWriteManager::postRead(common::Structure & structure,
    const ResourceLocator & locator) const
{
  // If the structure doesn't have a name, set it to the locator
  if(structure.getName().empty())
  {
    std::string name = io::stemString(locator.path());
    if(!locator.id().empty())
      name += "-" + locator.id();
    structure.setName(name);
  }

  structure.properties()[properties::io::LAST_ABS_FILE_PATH] = locator;
}

void
StructureReadWriteManager::postWrite(common::Structure & structure,
    const ResourceLocator & locator) const
{
  structure.properties()[properties::io::LAST_ABS_FILE_PATH] = locator;
}

}
}
