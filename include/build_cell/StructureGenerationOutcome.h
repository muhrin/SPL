/*
 * StructureGenerationOutcome.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_GENERATION_OUTCOME_H
#define STRUCTURE_GENERATION_OUTCOME_H

// INCLUDES ///////////////////////////////////////////////
#include "SSLib.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx
{
namespace build_cell
{

struct StructureGenerationOutcome
{
public:
  enum Value
  {
    UNDEFINED,
    FAILED_CREATING_UNIT_CELL,
    FAILED_SATISFYING_CONSTRAINTS,
    SUCCESS
  };
};

}}

#endif /* STRUCTURE_GENERATION_OUTCOME_H */
