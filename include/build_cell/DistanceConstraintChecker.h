/*
 * DistanceConstraintChecker.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef DISTANCE_CONSTRAINT_CHECKER_H
#define DISTANCE_CONSTRAINT_CHECKER_H

// INCLUDES ////////////
#include "build_cell/ConstStructureDescriptionVisitor.h"

namespace sstbx
{
// FORWARD DECLARES ////////////
namespace common
{
class Structure;
}

namespace build_cell
{

class StructureDescriptionMap;

class DistanceConstraintChecker : public ConstStructureDescriptionVisitor
{
public:

  DistanceConstraintChecker(const StructureDescriptionMap & descriptionMap);

  virtual bool visitAtomGroup(const AtomGroupDescription & groupDescription);

private:


  const StructureDescriptionMap &   myDescriptionMap;
  const common::Structure &         myStructure;

};

}
}

#endif /* DISTANCE_CONSTRAINT_CHECKER_H */
