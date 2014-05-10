/*
 * StructureBuilder.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////
#include "spl/build_cell/StructureBuilder.h"

#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "spl/build_cell/GenerationOutcome.h"
#include "spl/build_cell/IFragmentGenerator.h"
#include "spl/build_cell/IUnitCellGenerator.h"
#include "spl/build_cell/PointGroups.h"
#include "spl/build_cell/StructureBuild.h"
#include "spl/build_cell/StructureContents.h"
#include "spl/build_cell/SymmetryGroup.h"
#include "spl/common/Structure.h"
#include "spl/utility/IndexingEnums.h"

namespace spl {
namespace build_cell {

const double StructureBuilder::DEFAULT_ATOMS_OVERLAP = 0.1;
const bool StructureBuilder::DEFAULT_IS_CLUSTER = false;

StructureBuilder::StructureBuilder() :
    myPointGroup(PointGroupFamily::NONE, 0), myNumSymOps(0), myIsCluster(
        DEFAULT_IS_CLUSTER), myAtomsOverlap(DEFAULT_ATOMS_OVERLAP)
{
}

StructureBuilder::StructureBuilder(const ConstructInfo & info) :
    myPointGroup(PointGroupFamily::NONE, 0), myNumSymOps(0), myIsCluster(
        info.isCluster), myAtomsOverlap(info.atomsOverlap)
{
}

StructureBuilder::StructureBuilder(const StructureBuilder & toCopy) :
    StructureBuilderCore(toCopy), myPointGroup(toCopy.myPointGroup), myNumSymOps(
        toCopy.myNumSymOps), myIsCluster(toCopy.myIsCluster), myAtomsOverlap(
        toCopy.myAtomsOverlap), myUnitCellGenerator(
        myUnitCellGenerator->clone())
{
}

GenerationOutcome
StructureBuilder::generateStructure(common::StructurePtr & structureOut,
    const common::AtomSpeciesDatabase & speciesDb)
{
  GenerationOutcome outcome;

  typedef ::std::pair< const FragmentGenerator *,
      FragmentGenerator::GenerationTicket> GeneratorAndTicket;
  ::std::vector< GeneratorAndTicket> generationInfo;
  generationInfo.reserve(myGenerators.size());

  // First find out what the generators want to put in the structure
  FragmentGenerator::GenerationTicket structureTicket = getTicket();
  StructureContents contents = getGenerationContents(structureTicket,
      speciesDb);
  BOOST_FOREACH(FragmentGenerator & generator, myGenerators)
  {
    const FragmentGenerator::GenerationTicket ticket = generator.getTicket();
    generationInfo.push_back(GeneratorAndTicket(&generator, ticket));
    contents += generator.getGenerationContents(ticket, speciesDb);
  }

  structureOut.reset(new common::Structure());
  StructureBuild structureBuild(*structureOut, contents, myAtomsOverlap,
      speciesDb);
  if(!chooseSymmetry(structureBuild))
  {
    outcome.setFailure("Failed to generate a symmetry group");
    return outcome;
  }

  // Do we need to create a unit cell?
  if(myUnitCellGenerator.get())
  {
    common::UnitCellPtr cell;
    outcome = myUnitCellGenerator->generateCell(cell, contents, myIsCluster);

    if(!outcome.isSuccess())
      return outcome;

    if(cell.get())
      structureOut->setUnitCell(*cell);
    else
      return GenerationOutcome::failure(
          "Unit cell generator failed to generate unit cell");
  }

  // By now we should have a unit cell and symmetry if needed
  // so generate the atoms
  this->generateFragment(structureBuild, structureTicket, speciesDb);
  BOOST_FOREACH(const GeneratorAndTicket & generatorAndTicket, generationInfo)
  {
    outcome = generatorAndTicket.first->generateFragment(structureBuild,
        generatorAndTicket.second, speciesDb);

    if(!outcome.isSuccess())
      return outcome;
  }

  outcome = generateSymmetry(structureBuild);
  if(!outcome.isSuccess())
    return outcome;

  // TODO: Check global constraints

  outcome.setSuccess();
  return outcome;
}

GenerationOutcome StructureBuilder::generateStructure(
  common::StructurePtr & structureOut,
  const common::AtomSpeciesDatabase & speciesDb,
  const GenerationSettings & info
)
{
  setGenerationSettings(info);
  BOOST_FOREACH(FragmentGenerator & builder, myGenerators)
  {
    builder.setGenerationSettings(info);
  }
  const GenerationOutcome outcome = generateStructure(structureOut, speciesDb);
  clearGenerationSettings();
  return outcome;
}

void
StructureBuilder::setGenerationSettings(const GenerationSettings & settings)
{
  AbsAtomsGenerator::setGenerationSettings(settings);
  BOOST_FOREACH(FragmentGenerator & builder, myGenerators)
  {
    builder.setGenerationSettings(settings);
  }
}

void
StructureBuilder::setUnitCellGenerator(IUnitCellGeneratorPtr unitCellGenerator)
{
  myUnitCellGenerator = unitCellGenerator;
}

const IUnitCellGenerator *
StructureBuilder::getUnitCellGenerator() const
{
  return myUnitCellGenerator.get();
}

void
StructureBuilder::setPointGroup(const PointGroup & pointGroup)
{
  myPointGroup = pointGroup;
}

const PointGroup &
StructureBuilder::getPointGroup() const
{
  return myPointGroup;
}

bool
StructureBuilder::isCluster() const
{
  return myIsCluster;
}

bool
StructureBuilder::chooseSymmetry(StructureBuild & build) const
{
  if(myUnitCellGenerator.get())
  { // Crystal
    // TODO
  }
  else
  { // Cluster
    if(myPointGroup.first != PointGroupFamily::NONE)
    {
      StructureBuild::SymmetryGroupPtr group(new SymmetryGroup());
      generatePointGroup(*group, myPointGroup.first, myPointGroup.second);
      build.setSymmetryGroup(group);
    }
    else if(myNumSymOps != 0)
    {
      const ::boost::optional< PointGroup> pointGroup(
          getRandomPointGroup(myNumSymOps));
      if(!pointGroup)
        return false;
      StructureBuild::SymmetryGroupPtr group(new SymmetryGroup());
      generatePointGroup(*group, pointGroup->first, pointGroup->second);
      build.setSymmetryGroup(group);
    }
  }
  return true;
}

GenerationOutcome
StructureBuilder::generateSymmetry(StructureBuild & build) const
{
  using namespace utility::cart_coords_enum;
  // Pull in X Y Z as 0 1 2

  SSLIB_ASSERT(build.getStructure().getNumAtoms() == build.getNumAtomInfos());

  if(!build.getSymmetryGroup())
    return GenerationOutcome::success(); // No symmetry

  common::Structure & structure = build.getStructure();
  const common::UnitCell * const unitCell = structure.getUnitCell();
  const SymmetryGroup & group = *build.getSymmetryGroup();

  for(StructureBuild::AtomInfoIterator it = build.beginAtomInfo(), end =
      build.endAtomInfo(); it != end; ++it)
  {
    const BuildAtomInfo::OpMask & opMask = it->getOpMask();
    for(size_t op = 1 /*skip identity*/; op < group.numOps(); ++op)
    {
      if(opMask[op])
      {
        // Get the operator matrix
        ::arma::mat44 opMat(group.getOp(op));

        if(unitCell) // Transform the translation from fractional to absolute      
          opMat.col(3).rows(X, Z) = trans(
              unitCell->getOrthoMtx() * opMat.col(3).rows(X, Z));

        common::Atom & oldAtom = it->getAtom(0);
        ::arma::vec4 oldPosition;
        oldPosition.rows(X, Z) = oldAtom.getPosition();
        oldPosition(3) = 1.0; // <- To make symmetry translation work correctly
        // Apply the operator
        const ::arma::vec4 newPosition = opMat * oldPosition;

        // Make a copy of the old atom
        common::Atom & newAtom = structure.newAtom(oldAtom);
        // Set the new position
        newAtom.setPosition(::arma::vec3(newPosition.rows(X, Z)));

        // Tell the build that this is a copy of the old atom
        build.addAtom(newAtom, *it);
      }
    }
  }
  return GenerationOutcome::success();
}

}
}
