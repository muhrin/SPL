/*
 * CellReaderWriter.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef CELL_READER_WRITER_H
#define CELL_READER_WRITER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/io/IStructureReader.h"
#include "spl/io/IStructureWriter.h"

#include <ostream>
#include <vector>

#include <armadillo>

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class AtomSpeciesDatabase;
class Structure;
class UnitCell;
}

namespace io {

class CellReaderWriter : public virtual IStructureWriter,
    public virtual IStructureReader
{
public:
  static const unsigned int DIGITS_AFTER_DECIMAL;

  virtual std::vector< std::string>
  getSupportedFileExtensions() const;

  // From IStructureReader //
  virtual spl::common::types::StructurePtr
  readStructure(const ResourceLocator & resourceLocator) const;
  virtual size_t
  readStructures(StructuresContainer & outStructures,
      const ResourceLocator & resourceLocator) const;
  // End from IStructureReader //

  // from IStructureWriter //
  virtual void
  writeStructure(spl::common::Structure & str,
      const ResourceLocator & locator) const;
  // End from IStructureWriter

  // Single structure per file
  virtual bool
  multiStructureSupport() const
  {
    return false;
  }

  virtual common::types::StructurePtr
  readStructure(std::istream & is) const;

  virtual void
  writeStructure(std::ostream & os, common::Structure & structure) const;

  void
  writePressureBlock(std::ostream & os, const arma::mat33 & pressureMtx) const;

private:
  void
  writeLatticeBlock(std::ostream & os, const common::UnitCell & unitCell) const;
  void
  writePositionsBlock(std::ostream & os, const common::Structure & structure,
      const common::UnitCell & unitCell) const;
};

}
}

#endif /* CELL_READER_WRITER_H */
