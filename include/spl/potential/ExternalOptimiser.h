/*
 * ExternalOptimiser.h
 *
 *
 *  Created on: Jan 14, 2014
 *      Author: Martin Uhrin
 */

#ifndef EXTERNAL_OPTIMISER_H
#define EXTERNAL_OPTIMISER_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_YAML

#include <string>

#include "spl/potential/GeomOptimiser.h"
#include "spl/io/ResReaderWriter.h"

// DEFINES //////////////////////////////////////////////

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////

namespace potential {
class OptResults;

class ExternalOptimiser : public GeomOptimiser
{
public:
  ExternalOptimiser(const std::string & runCommand);
  virtual
  ~ExternalOptimiser()
  {
  }

  virtual OptimisationOutcome
  optimise(common::Structure & structure,
      const OptimisationSettings & options) const;
  virtual OptimisationOutcome
  optimise(common::Structure & structure, OptimisationData & data,
      const OptimisationSettings & options) const;

private:
  void
  writeSettings(const std::string & filename,
      const OptimisationSettings & settings) const;
  bool
  readResults(const std::string & filename, OptResults * const results) const;

  const std::string myRunCommand;
  const io::ResReaderWriter myResReaderWriter;
};

}
}

#endif // SPL_WITH_YAML
#endif /* EXTERNAL_OPTIMISER_H */
