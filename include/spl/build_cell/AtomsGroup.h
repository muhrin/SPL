/*
 * AtomsGroup.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOMS_GROUP_H
#define ATOMS_GROUP_H

// INCLUDES /////////////////////////////////

#include "spl/SSLib.h"

#include <boost/noncopyable.hpp>

#include "spl/build_cell/AbsAtomsGenerator.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {
class AtomsDescription;
class BuildAtomInfo;
struct AtomsGeneratorConstructionInfo;

class AtomsGroup : public AbsAtomsGenerator, ::boost::noncopyable
{
public:

  struct TransformMode
  {
    enum Value
    {
      FIXED = 0x00, // 000
      RAND_POS = 0x01, // 001
      RAND_ROT_DIR = 0x02, // 010
      RAND_ROT_ANGLE = 0x04 // 100
    };
  };

  AtomsGroup();
  AtomsGroup(const AtomsGroup & toCopy);

  unsigned int
  getNumReplicas() const;
  void
  setNumReplicas(const unsigned int numReplicas);

  int
  getTransformMode() const;
  void
  setTransformMode(const int mode);

  const ::arma::vec3 &
  getPosition() const;
  void
  setPosition(const ::arma::vec3 & pos);

  const ::arma::vec4 &
  getRotation() const;
  void
  setRotation(const ::arma::vec4 & rot);

  // From IFragmentGenerator ////////
  virtual GenerationOutcome
  generateFragment(StructureBuild & build, const GenerationTicket ticket,
      const common::AtomSpeciesDatabase & speciesDb) const;

  virtual StructureContents
  getGenerationContents(const GenerationTicket ticket,
      const common::AtomSpeciesDatabase & speciesDb) const;

  virtual IFragmentGeneratorPtr
  clone() const;
  // End from IFragmentGenerator

protected:

  virtual ::arma::mat44
  generateTransform(const StructureBuild & build) const;

  unsigned int myNumReplicas;
  int myTransformMode;
  ::arma::vec3 myPos;
  ::arma::vec4 myRot;
};

}
}

#endif /* ATOMS_GROUP_H */
