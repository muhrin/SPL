/*
 * AbsAbsAtomsGenerator.h
 *
 *  Created on: Jul 29, 2013
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////
#include "spl/build_cell/AbsAtomsGenerator.h"

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

AbsAtomsGenerator::AbsAtomsGenerator(const AbsAtomsGenerator & toCopy) :
    myAtoms(toCopy.myAtoms), myGenShape(toCopy.myGenShape->clone()),
    myLastTicketId(0)
{
}

void
AbsAtomsGenerator::insertAtoms(const AtomsDescription & atoms)
{
  myAtoms.push_back(atoms);
  // Invalidate all tickets
  myTickets.clear();
}

size_t
AbsAtomsGenerator::numAtoms() const
{
  return myAtoms.size();
}

AbsAtomsGenerator::const_iterator
AbsAtomsGenerator::atomsBegin() const
{
  return myAtoms.begin();
}

AbsAtomsGenerator::const_iterator
AbsAtomsGenerator::atomsEnd() const
{
  return myAtoms.end();
}

const GeneratorShape *
AbsAtomsGenerator::getGeneratorShape() const
{
  return myGenShape.get();
}

void
AbsAtomsGenerator::setGeneratorShape(UniquePtr< GeneratorShape>::Type genShape)
{
  myGenShape = genShape;
}

void
AbsAtomsGenerator::addSpeciesPairDistance(const SpeciesPair & pair,
    const double distance)
{
  mySpeciesPairDistances[pair] = distance;
}

GenerationOutcome
AbsAtomsGenerator::generateFragment(StructureBuild & build,
    const GenerationTicket ticket,
    const common::AtomSpeciesDatabase & speciesDb) const
{
  typedef ::std::vector< unsigned int> Multiplicities;

  // Push our current set of species pair distances onto the distances stack
  StructureBuild::SpeciesDistancesPusher distsPusher(build,
      mySpeciesPairDistances);

  // Get the ticket
  TicketsMap::const_iterator it = myTickets.find(ticket.getId());
  SSLIB_ASSERT_MSG(it != myTickets.end(),
      "Asked to build structure with ticket we don't recognise.");
  const AtomCounts & counts = it->second.atomCounts;
  if(counts.empty())
    return GenerationOutcome::success();

  GenerationOutcome outcome;

  common::Structure & structure = build.getStructure();

  // Do we have any symmetry?
  const bool usingSymmetry = build.getSymmetryGroup() != NULL;

  AtomPosition position;
  Multiplicities multiplicities;
  Multiplicities possibleMultiplicities;
  if(usingSymmetry)
  {
    possibleMultiplicities = build.getSymmetryGroup()->getMultiplicities();
    possibleMultiplicities.push_back(build.getSymmetryGroup()->numOps());
  }
  else
    possibleMultiplicities.push_back(1); // No symmetry so all points have multiplicity 1

  const ::arma::mat44 transform = generateTransform(build);

  BOOST_FOREACH(AtomCounts::const_reference atomsDesc, counts)
  {
    if(atomsDesc.first->position)
    {
      // If the atom has a fixed position then we should not apply symmetry
      // and the multiplicity should be 1
      // TODO: Check if this position is compatible with our symmetry operators!
      multiplicities.insert(multiplicities.begin(), atomsDesc.second, 1);
    }
    else
      multiplicities = symmetry::generateMultiplicities(atomsDesc.second,
          possibleMultiplicities);

    if(multiplicities.empty())
    {
      outcome.setFailure(
          "Couldn't factor atom multiplicities into number of sym ops.");
      return outcome;
    }

    // Go over the multiplicities inserting the atoms
    BOOST_FOREACH(const unsigned int multiplicity, multiplicities)
    {
      common::Atom & atom = structure.newAtom(atomsDesc.first->species);
      BuildAtomInfo & info = build.createAtomInfo(atom);
      info.setMultiplicity(multiplicity);

      position = generatePosition(info, *atomsDesc.first, build, multiplicity,
          transform);
      atom.setPosition(position.first);
      info.setFixed(position.second);

      atom.setRadius(getRadius(*atomsDesc.first, speciesDb));
    }
    multiplicities.clear(); // Reset for next loop
  }

  // Finally solve any atom overlap
  if(build.separateAtoms())
    outcome.setSuccess();
  else
    outcome.setFailure(
        "Failed to extrude atoms, maybe the volume is too small.");

  return outcome;
}

AbsAtomsGenerator::GenerationTicket
AbsAtomsGenerator::getTicket()
{
  GenerationTicket::IdType ticketId = ++myLastTicketId;

  // Generate a random number of atoms
  int count;
  bool countSet;
  AtomCounts counts;
  BOOST_FOREACH(const AtomsDescription & atom, myAtoms)
  {
    countSet = false;

    // First check if we have any generation settings that should override
    // the settings for this atom
    if(!atom.label.empty() && myGenerationSettings &&
        !myGenerationSettings->atomsSettings.empty())
    {
      const GenerationSettings::AtomsSettings::const_iterator atomsSettings =
          myGenerationSettings->atomsSettings.find(atom.label);
      if(atomsSettings != myGenerationSettings->atomsSettings.end())
      {
        count = atomsSettings->second.num;
        countSet = true;
      }
    }

    if(!countSet)
    {
      if(atom.count.nullSpan())
        count = atom.count.min();
      else
        count = math::randu(atom.count.min(), atom.count.max());
    }

    if(count != 0)
      counts[&atom] = count;
  }
  myTickets[ticketId].atomCounts = counts;
  return GenerationTicket(ticketId);
}

StructureContents
AbsAtomsGenerator::getGenerationContents(const GenerationTicket ticket,
    const common::AtomSpeciesDatabase & speciesDb) const
{
  StructureContents contents;

  // Get the ticket
  TicketsMap::const_iterator it = myTickets.find(ticket.getId());
  SSLIB_ASSERT_MSG(it != myTickets.end(),
      "Asked to build structure with unrecognised ticket.  Probably changed generator after the ticket was requested.");
  const AtomCounts & counts = it->second.atomCounts;

  double radius;
  BOOST_FOREACH(AtomCounts::const_reference atomsDesc, counts)
  {
    radius = getRadius(*atomsDesc.first, speciesDb);
    contents.addAtoms(static_cast< size_t>(atomsDesc.second), radius);
  }

  return contents;
}

void
AbsAtomsGenerator::setGenerationSettings(const GenerationSettings & settings)
{
  myGenerationSettings = settings;
  invalidateTickets();
}

void
AbsAtomsGenerator::clearGenerationSettings()
{
  if(!myGenerationSettings)
    return;

  myGenerationSettings.reset();
  invalidateTickets();
}

void
AbsAtomsGenerator::handleReleased(const GenerationTicketId & id)
{
  // Get the ticket
  TicketsMap::iterator it = myTickets.find(id);
  if(it == myTickets.end())
    return;

  myTickets.erase(it);
}

IFragmentGeneratorPtr
AbsAtomsGenerator::clone() const
{
  return IFragmentGeneratorPtr(new AbsAtomsGenerator(*this));
}

AbsAtomsGenerator::AtomPosition
AbsAtomsGenerator::generatePosition(BuildAtomInfo & atomInfo,
    const AtomsDescription & atom, const StructureBuild & build,
    const unsigned int multiplicity, const ::arma::mat44 & transformation) const
{
  SSLIB_ASSERT_MSG( !(multiplicity > 1 && !build.getSymmetryGroup()),
      "If we have a multiplicity of more than one then there must be a symmetry group");

  const bool usingSymmetry = build.getSymmetryGroup() != NULL;

  AtomPosition position;
  // Default is not fixed
  position.second = false;

  if(atom.position)
  {
    // Position is fixed
    position.first = math::transformCopy(*atom.position, transformation);
    position.second = true;
  }
  else
  {
    position.first = getGenShape(build).randomPoint(&transformation);

    // Do we need to apply symmetry and does the atom need to be on a 'special' position
    if(usingSymmetry)
    {
      if(multiplicity == build.getSymmetryGroup()->numOps())
      {
        // Apply all operators (true for entire op mask)
        atomInfo.setOperatorsMask(
            ::std::vector< bool>(build.getSymmetryGroup()->numOps(), true));
      }
      else // Special position
      {
        const SymmetryGroup::EigenspacesAndMasks * const spaces =
            build.getSymmetryGroup()->getEigenspacesAndMasks(multiplicity);

        ::arma::vec3 newPos;
        SymmetryGroup::OpMask opMask;
        if(!generateSpecialPosition(newPos, opMask, *spaces, getGenShape(build),
            transformation))
        {
          // TODO: return error
          return position;
        }

        // Save the position
        position.first = newPos;
        atomInfo.setOperatorsMask(opMask);
        position.second = true; // Fix the position so it doesn't get moved when extruding atoms
      }
    }
  }

  return position;
}

bool
AbsAtomsGenerator::generateSpecialPosition(::arma::vec3 & posOut,
    SymmetryGroup::OpMask & opMaskOut,
    const SymmetryGroup::EigenspacesAndMasks & spaces,
    const GeneratorShape & genShape,
    const ::arma::mat44 & transformation) const
{
  typedef ::std::vector< size_t> Indices;
  // Generate a list of the indices
  Indices indices;
  for(size_t i = 0; i < spaces.size(); ++i)
    indices.push_back(i);

  Indices::iterator it;
  OptionalArmaVec3 pos;
  while(!indices.empty())
  {
    // Get a random one in the list
    it = indices.begin() + math::randu< size_t>(indices.size() - 1);
    pos = generateSpeciesPosition(spaces[*it].first, genShape, transformation);
    if(pos)
    {
      // Copy over the position and mask
      posOut = *pos;
      opMaskOut = spaces[*it].second;
      return true;
    }
    indices.erase(it);
  }
  return false; // Couldn't find one
}

OptionalArmaVec3
AbsAtomsGenerator::generateSpeciesPosition(
    const SymmetryGroup::Eigenspace & space, const GeneratorShape & genShape,
    const ::arma::mat44 & transformation) const
{
  // Select the correct function depending on the number of eigenvectors
  if(space.n_cols == 1)
    return genShape.randomPointOnAxis(space, &transformation);
  else if(space.n_cols == 2)
    return genShape.randomPointInPlane(space.col(0), space.col(1),
        &transformation);
  return OptionalArmaVec3(::arma::zeros< ::arma::vec>(3));
}

double
AbsAtomsGenerator::getRadius(const AtomsDescription & atom,
    const common::AtomSpeciesDatabase & speciesDb) const
{
  double radius = 0.0;
  ::boost::optional< double> optionalRadius;

  // Try getting it from the atom
  if(atom.radius)
    radius = *atom.radius;
  else
  {
    // Then try from the database
    optionalRadius = speciesDb.getRadius(atom.species);
    if(optionalRadius)
      radius = *optionalRadius;
  }

  return radius;
}

const GeneratorShape &
AbsAtomsGenerator::getGenShape(const StructureBuild & build) const
{
  if(myGenShape.get())
    return *myGenShape;
  else
    return build.getGenShape();
}

::arma::mat44
AbsAtomsGenerator::generateTransform(const StructureBuild & build) const
{
  return ::arma::eye(4, 4);
}

void
AbsAtomsGenerator::invalidateTickets()
{
  myTickets.clear();
}

}
}
