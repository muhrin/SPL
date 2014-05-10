/*
 * StructureReadWriteManager.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_READ_WRITE_MANAGER_DETAIL_H
#define STRUCTURE_READ_WRITE_MANAGER_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <boost/type_traits.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
namespace io {
class IStructureWriter;
class ResourceLocator;
}
}


namespace spl {
namespace io {
namespace detail {

template <class ReaderOrWriter, bool isReader, bool isWriter>
struct DoInsert
{
};

// Reader specialisation
template <class Reader>
struct DoInsert<Reader, true, false>
{
  static Reader & insert(
    StructureReadWriteManager & rwMan,
    typename UniquePtr<Reader>::Type reader)
  {
    // Save a 'weak' pointer to the reader so we can return it
    Reader * const readerPtr = reader.get();
    rwMan.insertReader(StructureReadWriteManager::ReaderPtr(reader));
    return *readerPtr;
  }
};

// Writer specialisation
template <class Writer>
struct DoInsert<Writer, false, true>
{
  static Writer & insert(
    StructureReadWriteManager & rwMan,
    typename UniquePtr<Writer>::Type writer)
  {
    Writer * const writerPtr = writer.get();
    rwMan.insertWriter(StructureReadWriteManager::WriterPtr(writer));
    return *writerPtr;
  }
};

// Reader/Writer specialisation
template <class ReaderWriter>
struct DoInsert<ReaderWriter, true, true>
{
  static ReaderWriter & insert(
    StructureReadWriteManager & rwMan,
    typename UniquePtr<ReaderWriter>::Type readWriter)
  {
    ReaderWriter * const readWriterPtr = readWriter.get();
    rwMan.insertReader(StructureReadWriteManager::ReaderPtr(readWriter));
    rwMan.registerWriter(*readWriterPtr);
    return *readWriterPtr;
  }
};

}

template <class ReaderOrWriter>
ReaderOrWriter & StructureReadWriteManager::insert(SSLIB_UNIQUE_PTR(ReaderOrWriter) readerOrWriter)
{
  return detail::DoInsert<ReaderOrWriter,
    ::boost::is_base_of<IStructureReader, ReaderOrWriter>::value,
    ::boost::is_base_of<IStructureWriter, ReaderOrWriter>::value >::insert(*this, readerOrWriter);
}

}
}

#endif /* STRUCTURE_READ_WRITE_MANAGER_DETAIL_H */
