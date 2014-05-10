/*
 * AtomsGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOMS_GENERATOR_H
#define ATOMS_GENERATOR_H

// INCLUDES /////////////////////////////////

#include "spl/SSLib.h"

#include <boost/noncopyable.hpp>

#include "spl/OptionalTypes.h"
#include "spl/build_cell/IFragmentGenerator.h"
#include "spl/build_cell/GeneratorShape.h"
#include "spl/build_cell/SymmetryGroup.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {
class AtomsDescription;
class BuildAtomInfo;
struct AtomsGeneratorConstructionInfo;

class AtomsGenerator : public FragmentGenerator, ::boost::noncopyable
{
  typedef FragmentGenerator::GenerationTicketId GenerationTicketId;
  typedef ::std::vector<AtomsDescription> Atoms;
public:
  typedef FragmentGenerator::GenerationTicket GenerationTicket;
  typedef Atoms::iterator iterator;
  typedef Atoms::const_iterator const_iterator;
  typedef UniquePtr<GeneratorShape>::Type GenShapePtr;

  struct TransformMode
  {
    enum Value
    {
      FIXED         = 0x00, // 000
      RAND_POS      = 0x01, // 001
      RAND_ROT_DIR  = 0x02, // 010
      RAND_ROT_ANGLE= 0x04  // 100
    };
  };

  AtomsGenerator();
  AtomsGenerator(const AtomsGenerator & toCopy);

  void insertAtoms(const AtomsDescription & atoms);
  size_t numAtoms() const;
  const_iterator beginAtoms() const;
  const_iterator endAtoms() const;

  const GeneratorShape * getGeneratorShape() const;
  void setGeneratorShape(UniquePtr<GeneratorShape>::Type genShape);
  
  int getNumReplicas() const;
  void setNumReplicas(const int numReplicas);

  int getTransformMode() const;
  void setTransformMode(const int mode);

  const ::arma::vec3 & getPosition() const;
  void setPosition(const ::arma::vec3 & pos);

  const ::arma::vec4 & getRotation() const;
  void setRotation(const ::arma::vec4 & rot);

  // From IFragmentGenerator ////////
  virtual GenerationOutcome generateFragment(
    StructureBuild & build,
    const GenerationTicket ticket,
    const common::AtomSpeciesDatabase & speciesDb
  ) const;

  virtual GenerationTicket getTicket();
  virtual StructureContents getGenerationContents(
    const GenerationTicket ticket,
    const common::AtomSpeciesDatabase & speciesDb
  ) const;

  virtual void handleReleased(const GenerationTicketId & id);

  virtual IFragmentGeneratorPtr clone() const;
  // End from IFragmentGenerator

private:
  typedef ::std::pair< ::arma::vec3, bool> AtomPosition;
  typedef ::std::map<const AtomsDescription *, int> AtomCounts;

  struct GenerationInfo
  {
    AtomCounts atomCounts;
    ::arma::mat44 shapeTransform;
  };

  typedef ::std::map<GenerationTicket::IdType, GenerationInfo> TicketsMap;

  AtomPosition generatePosition(
    BuildAtomInfo & atomInfo,
    const AtomsDescription & atom,
    const StructureBuild & build,
    const unsigned int multiplicity,
    const ::arma::mat44 & transformation
  ) const;
  bool generateSpecialPosition(
    ::arma::vec3 & posOut,
    SymmetryGroup::OpMask & opMaskOut,
    const SymmetryGroup::EigenspacesAndMasks & spaces,
    const GeneratorShape & genShape,
    const ::arma::mat44 & transformation
  ) const;
  OptionalArmaVec3 generateSpeciesPosition(
    const SymmetryGroup::Eigenspace & eigenspace,
    const GeneratorShape & genShape,
    const ::arma::mat44 & transformation
  ) const;
  double getRadius(const AtomsDescription & atom, const common::AtomSpeciesDatabase & speciesDb) const;

  const GeneratorShape & getGenShape(const StructureBuild & build) const;

  ::arma::mat44 generateTransform(const StructureBuild & build) const;

  Atoms myAtoms;
  UniquePtr<GeneratorShape>::Type myGenShape;
  unsigned int myNumReplicas;
  int myTransformMode;
  ::arma::vec3 myPos;
  ::arma::vec4 myRot;
  TicketsMap myTickets;
  GenerationTicket::IdType myLastTicketId;
};


}
}

#endif /* ATOMS_GENERATOR_H */
