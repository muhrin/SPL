/*
 * DistanceConstraintChecker.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "build_cell/DistanceConstraintChecker.h"

#include <armadillo>

#include "build_cell/AtomGroupDescription.h"
#include "build_cell/AtomsDescription.h"
#include "build_cell/StructureDescriptionMap.h"
#include "common/Atom.h"
#include "common/DistanceCalculator.h"
#include "common/Structure.h"


namespace sstbx {
namespace build_cell {

DistanceConstraintChecker::DistanceConstraintChecker(const StructureDescriptionMap & descriptionMap):
myDescriptionMap(descriptionMap),
myStructure(descriptionMap.getStructure())
{}

bool DistanceConstraintChecker::visitAtomGroup(const AtomGroupDescription & groupDescription)
{

  // First get all the atom and description pairs for this group
  StructureDescriptionMap::ConstDescriptionsAndAtoms groupAtoms;
  myDescriptionMap.getDescriptionsAndAtoms(&groupDescription, groupAtoms);
  const size_t numGroupAtoms = groupAtoms.size();

  StructureDescriptionMap::ConstDescriptionsAndAtoms groupAtomsAndBelow;
  myDescriptionMap.getDescriptionsAndAtoms(&groupDescription, groupAtomsAndBelow, AtomGroupDescription::DEPTH_ALL_CHILD_GROUPS);

  // Now calculate distance limits matrix
  ::arma::mat minDistancesSq(groupAtoms.size(), groupAtomsAndBelow.size());

  ::boost::optional<double> radius;
  const common::DistanceCalculator & distCalc = myStructure.getDistanceCalculator();
  double iRadius, minDistSq;
  ::arma::vec3 iPos;
  for(size_t i = 0; i < numGroupAtoms; ++i)
  {
    radius = groupAtoms[i].first->getRadius();
    iRadius = radius ? *radius : 0.0;
    iPos = groupAtoms[i].second->getPosition();

    // Do atoms up to i (exclusive)
    for(size_t j = 0; j < i; ++j)
    {
      minDistSq = iRadius;

      radius = groupAtomsAndBelow[j].first->getRadius();
      if(radius)
      {
        minDistSq += *radius;
      }

      if(minDistSq > 0.0)
      {
        // Square the minimum distance
        minDistSq *= minDistSq;
        if(distCalc.getDistSqMinImg(
          iPos,
          groupAtoms[j].second->getPosition()) <
          minDistSq)
        {
          return false;
        }
      }
    }

    // Do atoms after i
    for(size_t j = i + 1; j < groupAtomsAndBelow.size(); ++j)
    {
      minDistSq = iRadius;

      radius = groupAtomsAndBelow[j].first->getRadius();
      if(radius)
      {
        minDistSq += *radius;
      }

      if(minDistSq > 0.0)
      {
        // Square the minimum distance
        minDistSq *= minDistSq;
        if(distCalc.getDistSqMinImg(
          iPos,
          groupAtoms[j].second->getPosition()) <
          minDistSq)
        {
          return false;
        }
      }
    }
  }
  
  return true;
}


}
}
