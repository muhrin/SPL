/*
 * FragmentGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef FRAGMENT_GENERATOR_H
#define FRAGMENT_GENERATOR_H

// INCLUDES ////////////
#include "spl/build_cell/BuildCellFwd.h"

#include "spl/build_cell/GenerationSettings.h"
#include "spl/build_cell/GenerationOutcome.h"
#include "spl/utility/SharedHandle.h"

// DEFINITION ///////////////////////

namespace spl {
namespace common {
class AtomSpeciesDatabase;
}
namespace build_cell {
// FORWARD DECLARATIONS ///////
class StructureBuild;
class StructureContents;

class FragmentGenerator : utility::SharedHandle< ptrdiff_t>::Notifiee
{
protected:
  typedef ptrdiff_t GenerationTicketId;
public:
  typedef utility::SharedHandle< GenerationTicketId> GenerationTicket;

  virtual
  ~FragmentGenerator()
  {
  }

  virtual GenerationOutcome
  generateFragment(StructureBuild & build, const GenerationTicket ticket,
      const common::AtomSpeciesDatabase & speciesDb) const = 0;

  virtual GenerationTicket
  getTicket() = 0;
  virtual StructureContents
  getGenerationContents(const GenerationTicket ticket,
      const common::AtomSpeciesDatabase & speciesDb) const = 0;

  virtual void
  handleReleased(const GenerationTicketId & id) = 0;

  virtual void
  setGenerationSettings(const GenerationSettings & settings) = 0;
  virtual void
  clearGenerationSettings() = 0;

  virtual IFragmentGeneratorPtr
  clone() const = 0;
};

// Support for boost ptr_container copy construction.
inline FragmentGenerator *
new_clone(const FragmentGenerator & toClone)
{
  return toClone.clone().release();
}

}
}

#endif /* FRAGMENT_GENERATOR_H */
