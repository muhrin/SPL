/*
 * AtomsGroup.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES /////////////////
#include "spl/build_cell/AtomsGroup.h"

#include <boost/foreach.hpp>

#include "spl/build_cell/AtomsDescription.h"
#include "spl/build_cell/GenerationOutcome.h"
#include "spl/build_cell/StructureBuild.h"
#include "spl/build_cell/StructureContents.h"
#include "spl/build_cell/SymmetryFunctions.h"
#include "spl/common/Atom.h"
#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/Constants.h"
#include "spl/common/Structure.h"
#include "spl/math/Matrix.h"
#include "spl/math/Random.h"
#include "spl/utility/IndexingEnums.h"
#include "spl/utility/SharedHandle.h"

namespace spl {
namespace build_cell {

AtomsGroup::AtomsGroup():
myNumReplicas(1),
myTransformMode(TransformMode::FIXED)
{
  myPos.zeros();

  myRot.zeros();
  myRot(2) = 1.0; // Need this so we don't get NaNs in calculating roation
}

AtomsGroup::AtomsGroup(const AtomsGroup & toCopy):
AbsAtomsGenerator(toCopy),
myNumReplicas(toCopy.myNumReplicas),
myTransformMode(toCopy.myTransformMode),
myPos(toCopy.myPos),
myRot(toCopy.myRot)
{}

unsigned int AtomsGroup::getNumReplicas() const
{
  return myNumReplicas;
}

void AtomsGroup::setNumReplicas(const unsigned int numReplicas)
{
  if(myNumReplicas == numReplicas)
    return;

  myNumReplicas = numReplicas;
  invalidateTickets();
}

int AtomsGroup::getTransformMode() const
{
  return myTransformMode;
}

void AtomsGroup::setTransformMode(const int mode)
{
  myTransformMode = mode;
}

const ::arma::vec3 & AtomsGroup::getPosition() const
{
  return myPos;
}

void AtomsGroup::setPosition(const ::arma::vec3 & pos)
{
  myPos = pos;
}

const ::arma::vec4 & AtomsGroup::getRotation() const
{
  return myRot;
}

void AtomsGroup::setRotation(const ::arma::vec4 & rot)
{
  myRot = rot;
}

GenerationOutcome
AtomsGroup::generateFragment(
  StructureBuild & build,
  const GenerationTicket ticket,
  const common::AtomSpeciesDatabase & speciesDb
) const
{
  GenerationOutcome outcome;
  for(unsigned int i = 0; i < myNumReplicas; ++i)
  {
    outcome = AbsAtomsGenerator::generateFragment(build, ticket, speciesDb);
    if(!outcome.isSuccess())
      return outcome;
  }
  return outcome;
}

StructureContents AtomsGroup::getGenerationContents(
  const GenerationTicket ticket,
  const common::AtomSpeciesDatabase & speciesDb
) const
{
  StructureContents contents = AbsAtomsGenerator::getGenerationContents(ticket, speciesDb);
  contents *= myNumReplicas;
  return contents;
}

IFragmentGeneratorPtr AtomsGroup::clone() const
{
  return IFragmentGeneratorPtr(new AtomsGroup(*this));
}

::arma::mat44 AtomsGroup::generateTransform(const StructureBuild & build) const
{
  using namespace utility::cart_coords_enum;

  ::arma::vec4 axisAngle = myRot;
  if(myTransformMode & TransformMode::RAND_ROT_DIR)
    axisAngle.rows(X, Z) = math::normaliseCopy(::arma::randu< ::arma::vec>(3));
  if(myTransformMode & TransformMode::RAND_ROT_ANGLE)
    axisAngle(3) = math::randu(0.0, common::constants::TWO_PI);

  ::arma::vec3 pos = myPos;
  if(myTransformMode & TransformMode::RAND_POS)
  {
    // Create a random point somewhere within the global generation shape
    pos = build.getGenShape().randomPoint();
  }

  ::arma::mat44 transform;
  transform.eye();
  math::setTranslation(transform, pos);
  math::setRotation(transform, axisAngle);

  return transform;
}

}
}
