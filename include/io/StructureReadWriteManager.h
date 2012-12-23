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
#include "SSLib.h"

#include <map>

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

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

class StructureReadWriteManager
{
public:

  typedef UniquePtr<IStructureWriter>::Type WriterPtr;
  typedef UniquePtr<IStructureReader>::Type ReaderPtr;

  template <class ReaderOrWriter>
  ReaderOrWriter & insert(SSLIB_UNIQUE_PTR(ReaderOrWriter) & readerOrWriter);
  void insertReader(ReaderPtr reader);
  void insertWriter(WriterPtr writer);

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
  typedef ::boost::ptr_vector<IStructureWriter> WritersStore;
  typedef ::boost::ptr_vector<IStructureReader> ReadersStore;

  bool getExtension(::std::string & ext, const ResourceLocator & locator) const;

	WritersMap myWriters;
  ReadersMap myReaders;

  WritersStore myWritersStore;
  ReadersStore myReadersStore;

};

}
}

#include "io/detail/StructureReadWriteManager.h"

#endif /* STRUCTURE_WRITER_MANAGER_H */
