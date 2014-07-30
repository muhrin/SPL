/*
 * StructureReadWriteManager.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_READ_WRITE_MANAGER_H
#define STRUCTURE_READ_WRITE_MANAGER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <map>

#ifdef SSLIB_ENABLE_THREAD_AWARE
#  include <boost/thread/mutex.hpp>
#endif

#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/range/iterator_range.hpp>

#include "spl/common/Types.h"
#include "spl/io/IStructureReader.h"
#include "spl/io/IStructureWriter.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class Structure;
}
namespace io {
class IStructureWriter;
class ResourceLocator;
}
}

namespace spl {
namespace io {

class StructureReadWriteManager : boost::noncopyable
{
  typedef std::map< std::string, IStructureWriter *> WritersMap;
  typedef std::map< std::string, IStructureReader *> ReadersMap;

public:
  typedef UniquePtr< IStructureWriter>::Type WriterPtr;
  typedef UniquePtr< IStructureReader>::Type ReaderPtr;
  typedef WritersMap::iterator WritersIterator;
  typedef WritersMap::const_iterator WritersConstIterator;
  typedef ReadersMap::iterator ReadersIterator;
  typedef ReadersMap::const_iterator ReadersConstIterator;
  typedef boost::iterator_range< WritersIterator> WritersRange;
  typedef boost::iterator_range< WritersConstIterator> WritersConstRange;
  typedef boost::iterator_range< ReadersIterator> ReadersRange;
  typedef boost::iterator_range< ReadersConstIterator> ReadersConstRange;

  StructureReadWriteManager();

  WritersIterator
  beginWriters();
  WritersConstIterator
  beginWriters() const;
  WritersIterator
  endWriters();
  WritersConstIterator
  endWriters() const;
  WritersRange
  writers();
  WritersConstRange
  writers() const;
  size_t
  numWriters() const;

  ReadersIterator
  beginReaders();
  ReadersConstIterator
  beginReaders() const;
  ReadersIterator
  endReaders();
  ReadersConstIterator
  endReaders() const;
  ReadersRange
  readers();
  ReadersConstRange
  readers() const;
  size_t
  numReaders() const;

  template< class ReaderOrWriter>
    ReaderOrWriter &
    insert(SSLIB_UNIQUE_PTR(ReaderOrWriter)readerOrWriter);
    void insertReader(ReaderPtr reader);
    void insertWriter(WriterPtr writer);

    void registerWriter(IStructureWriter & writer);
    void deregisterWriter(IStructureWriter & writer);

    void registerReader(IStructureReader & reader);
    void deregisterReader(IStructureReader & reader);

    bool writeStructure(
    common::Structure & str,
    ResourceLocator locator) const;

    bool writeStructure(
    common::Structure & str,
    ResourceLocator locator,
    const std::string & fileType) const;

    common::types::StructurePtr readStructure(const ResourceLocator & locator)const;

  size_t readStructures(
    StructuresContainer & outStructures,
    const ResourceLocator & locator,
    const int maxDepth = 1
  ) const;

  const IStructureWriter * getWriter(const std::string & extension) const;

  bool setDefaultWriter(const std::string & extension);
  const IStructureWriter * getDefaultWriter() const;

private:
  typedef boost::ptr_vector<IStructureWriter> WritersStore;
  typedef boost::ptr_vector<IStructureReader> ReadersStore;

  bool getExtension(std::string & ext, const ResourceLocator & locator) const;

  size_t doReadAllStructuresFromPath(
    StructuresContainer & outStructures,
    const boost::filesystem::path & path,
    const size_t maxDepth,
    const size_t currentDepth = 0) const;

  void postRead(common::Structure & structure, const ResourceLocator & locator) const;
  void postWrite(common::Structure & structure, const ResourceLocator & locator) const;

  std::string myDefaultWriteExtension;

  WritersMap myWriters;
  ReadersMap myReaders;

  WritersStore myWritersStore;
  ReadersStore myReadersStore;

#ifdef SSLIB_ENABLE_THREAD_AWARE
  // TODO: Check if std::vector is thread safe, otherwise need mutexes for
  // inserting/removing readers and writers
  // TODO: Currently we lock for all read and writes but we should be locking
  // only for read and writes to the same file
  mutable boost::mutex myRwMutex;
#endif
};

}
}

#include "spl/io/detail/StructureReadWriteManager.h"

#endif /* STRUCTURE_WRITER_MANAGER_H */
