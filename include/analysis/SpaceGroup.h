/*
 * SpaceGroup.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SPACE_GROUP_H
#define SPACE_GROUP_H

// INCLUDES ////////////
#include "SSLib.h"

#include <string>



// DEFINITION ///////////////////////

namespace sstbx {

// FORWARD DECLARATIONS ///////
namespace common {
  class Structure;
}

namespace analysis {
namespace space_group {

static const double DEFAULT_PRECISION = 0.05;

struct SpacegroupInfo
{
  unsigned int number;
  ::std::string iucSymbol;
  ::std::string hallSymbol;
};

bool getSpacegroupInfo(
  SpacegroupInfo & outInfo,
  const common::Structure & structure,
  const double precision = DEFAULT_PRECISION);



}
}
}

#endif /* SPACE_GROUP_H */
