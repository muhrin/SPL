/*
 * AbsAtomsGenerator.h
 *
 *  Created on: Jul 29, 2013
 *      Author: Martin Uhrin
 */

#ifndef ABS_ATOMS_GENERATOR_H
#define ABS_ATOMS_GENERATOR_H

// INCLUDES /////////////////////////////////

#include "spl/SSLib.h"

#include <map>

#include "spl/OptionalTypes.h"
#include "spl/build_cell/AtomsDescription.h"
#include "spl/build_cell/IFragmentGenerator.h"
#include "spl/build_cell/GeneratorShape.h"
#include "spl/build_cell/SymmetryGroup.h"
#include "spl/utility/Range.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {
typedef utility::OrderedPair< std::string> SpeciesPair;
class BuildAtomInfo;
struct AtomsGeneratorConstructionInfo;
class StructureBuild;


class AbsAtomsGenerator : public FragmentGenerator
{
  typedef FragmentGenerator::GenerationTicketId GenerationTicketId;
  typedef ::std::vector< AtomsDescription> Atoms;
public:
  typedef FragmentGenerator::GenerationTicket GenerationTicket;
  typedef Atoms::const_iterator const_iterator;
  typedef Atoms::const_iterator iterator;
  typedef UniquePtr< GeneratorShape>::Type GenShapePtr;

  void
  insertAtoms(const AtomsDescription & atoms);
  size_t
  numAtoms() const;
  const_iterator
  atomsBegin() const;
  const_iterator
  atomsEnd() const;

  const GeneratorShape *
  getGeneratorShape() const;
  void
  setGeneratorShape(UniquePtr< GeneratorShape>::Type genShape);

  void
  addSpeciesPairDistance(const SpeciesPair & pair, const double distance);

  // From IFragmentGenerator ////////
  virtual GenerationOutcome
  generateFragment(StructureBuild & build, const GenerationTicket ticket,
      const common::AtomSpeciesDatabase & speciesDb) const;

  virtual GenerationTicket
  getTicket();
  virtual StructureContents
  getGenerationContents(const GenerationTicket ticket,
      const common::AtomSpeciesDatabase & speciesDb) const;

  virtual void
  setGenerationSettings(const GenerationSettings & settings);
  virtual void
  clearGenerationSettings();

  virtual void
  handleReleased(const GenerationTicketId & id);

  virtual IFragmentGeneratorPtr
  clone() const;
  // End from IFragmentGenerator

protected:
  AbsAtomsGenerator() :
      myLastTicketId(0)
  {
  }
  AbsAtomsGenerator(const AbsAtomsGenerator & toCopy);

  virtual ::arma::mat44
  generateTransform(const StructureBuild & build) const;

  void
  invalidateTickets();

private:
  typedef ::std::pair< ::arma::vec3, bool> AtomPosition;
  typedef ::std::map< const AtomsDescription *, int> AtomCounts;
  typedef ::std::map< SpeciesPair, double> SpeciesPairDistances;

  struct GenerationInfo
  {
    AtomCounts atomCounts;
    ::arma::mat44 shapeTransform;
  };

  typedef ::std::map< GenerationTicket::IdType, GenerationInfo> TicketsMap;

  AtomPosition
  generatePosition(BuildAtomInfo & atomInfo, const AtomsDescription & atom,
      const StructureBuild & build, const unsigned int multiplicity,
      const ::arma::mat44 & transformation) const;
  bool
  generateSpecialPosition(::arma::vec3 & posOut,
      SymmetryGroup::OpMask & opMaskOut,
      const SymmetryGroup::EigenspacesAndMasks & spaces,
      const GeneratorShape & genShape,
      const ::arma::mat44 & transformation) const;
  OptionalArmaVec3
  generateSpeciesPosition(const SymmetryGroup::Eigenspace & eigenspace,
      const GeneratorShape & genShape,
      const ::arma::mat44 & transformation) const;
  double
  getRadius(const AtomsDescription & atom,
      const common::AtomSpeciesDatabase & speciesDb) const;

  const GeneratorShape &
  getGenShape(const StructureBuild & build) const;

  Atoms myAtoms;
  UniquePtr< GeneratorShape>::Type myGenShape;
  TicketsMap myTickets;
  GenerationTicket::IdType myLastTicketId;
  SpeciesPairDistances mySpeciesPairDistances;
  ::boost::optional< GenerationSettings> myGenerationSettings;
};

}
}

#endif /* ABS_ATOMS_GENERATOR_H */
