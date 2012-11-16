/*
 * RandomAtomPositioner.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef RANDOM_ATOM_POSITIONER_H
#define RANDOM_ATOM_POSITIONER_H

// INCLUDES ///////////////////////////////////
#include <vector>

#include <armadillo>

#include "build_cell/ConstStructureDescriptionVisitor.h"


// FORWARD DECLARES ///////////////////////////


namespace sstbx
{
namespace common
{
class Structure;
}

namespace build_cell
{
class StructureDescriptionMap;

class RandomAtomPositioner : public ConstStructureDescriptionVisitor
{
public:

  RandomAtomPositioner(StructureDescriptionMap & descriptionMap);

  // From StructureTreeVisitor /////////////
  virtual void enteringAtomGroup(const AtomGroupDescription & atomGroup);
  virtual void leavingAtomGroup(const AtomGroupDescription & atomGroup);
  virtual bool visitAtomGroup(const AtomGroupDescription & atomGroup);
  // End from StructureTreeVisitor /////////

private:

  typedef ::std::vector< ::arma::vec> DisplacementsStack;

  void pushDisplacement(const ::arma::vec3 & displacement);
  void popDisplacement();

  bool checkGroupConstraints(const AtomGroupDescription & atomGroup);

  StructureDescriptionMap &        myDescriptionMap;
  ::sstbx::common::Structure &     myStructure;

  DisplacementsStack               myDisplacementsStack;
  ::arma::vec3                     myCurrentPosition;


};

}
}

#endif /* RANDOM_ATOM_POSITIONER_H */
