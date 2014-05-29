/*
 * StructureBuild.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/StructureBuild.h"

#include <boost/foreach.hpp>

#include "spl/OptionalTypes.h"
#include "spl/build_cell/BuildAtomInfo.h"
#include "spl/build_cell/GenSphere.h"
#include "spl/build_cell/Shapes.h"
#include "spl/build_cell/StructureContents.h"
#include "spl/build_cell/SymmetryGroup.h"
#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/Constants.h"
#include "spl/common/Structure.h"
#include "spl/common/UnitCell.h"
#include "spl/math/Random.h"

namespace spl {
namespace build_cell {

const double StructureBuild::RadiusCalculator::FALLBACK_RADIUS = 10.0;

StructureBuild::RadiusCalculator::RadiusCalculator(const double radius_,
    const bool isMultiplier_) :
    radius(radius_), isMultiplier(isMultiplier_)
{
}

double
StructureBuild::RadiusCalculator::getRadius(const double vol) const
{
  if(isMultiplier)
  {
    if(vol != 0.0)
      return radius * sphere::radius(vol);
    else
      return FALLBACK_RADIUS;
  }
  else
    return radius;
}

StructureBuild::StructureBuild(common::Structure & structure,
    const StructureContents & intendedContents, const double atomsOverlap,
    const common::AtomSpeciesDatabase & speciesDb,
    const RadiusCalculator & radiusCalculator) :
    myStructure(structure), myIntendedContents(intendedContents), mySpeciesDb(
        speciesDb), myAtomsOverlap(atomsOverlap)
{
  myGenShape.reset(
      new GenSphere(
          radiusCalculator.getRadius(myIntendedContents.getVolume())));
  myTransform.eye();
  myTransformCurrent = true;
  mySpeciesPairDistancesCurrent = true;
}

StructureBuild::StructureBuild(common::Structure & structure,
    const StructureContents & intendedContents, const double atomsOverlap,
    const common::AtomSpeciesDatabase & speciesDb, GenShapePtr genShape) :
    myStructure(structure), myIntendedContents(intendedContents), mySpeciesDb(
        speciesDb), myGenShape(genShape), myAtomsOverlap(atomsOverlap)
{
  myTransform.eye();
  myTransformCurrent = true;
  mySpeciesPairDistancesCurrent = true;
}

common::Structure &
StructureBuild::getStructure()
{
  return myStructure;
}

const common::Structure &
StructureBuild::getStructure() const
{
  return myStructure;
}

size_t
StructureBuild::getNumAtomInfos() const
{
  return myAtomsInfo.size();
}

StructureBuild::AtomInfoIterator
StructureBuild::beginAtomInfo()
{
  return myAtomInfoList.begin();
}

StructureBuild::AtomInfoIterator
StructureBuild::endAtomInfo()
{
  return myAtomInfoList.end();
}

BuildAtomInfo *
StructureBuild::getAtomInfo(common::Atom & atom)
{
  const AtomInfoMap::iterator it = myAtomsInfo.find(&atom);

  if(it == myAtomsInfo.end())
    return NULL;

  return it->second;
}

const BuildAtomInfo *
StructureBuild::getAtomInfo(const common::Atom & atom) const
{
  const AtomInfoMap::const_iterator it = myAtomsInfo.find(&atom);

  if(it == myAtomsInfo.end())
    return NULL;

  return it->second;
}

BuildAtomInfo &
StructureBuild::createAtomInfo(common::Atom & atom)
{
  BuildAtomInfo & atomInfo = *myAtomInfoList.insert(myAtomInfoList.end(),
      new BuildAtomInfo(atom));
  myAtomsInfo[&atom] = &atomInfo;
  return atomInfo;
}

void
StructureBuild::addAtom(common::Atom & atom, BuildAtomInfo & atomInfo)
{
  myAtomsInfo[&atom] = &atomInfo;
}

void
StructureBuild::removeAtom(common::Atom & atom)
{
  AtomInfoMap::iterator itMap = myAtomsInfo.find(&atom);
  SSLIB_ASSERT(itMap != myAtomsInfo.end());

  BuildAtomInfo & atomInfo = *itMap->second;
  AtomInfoList::iterator itList = myAtomInfoList.begin();
  for(AtomInfoList::iterator end = myAtomInfoList.end(); itList != end;
      ++itList)
  {
    if(&(*itList) == &atomInfo)
      break;
  }
  SSLIB_ASSERT(itList != myAtomInfoList.end());

  myAtomsInfo.erase(itMap);
  if(atomInfo.getNumAtoms() == 0)
    myAtomInfoList.erase(itList);
}

const GeneratorShape &
StructureBuild::getGenShape() const
{
  return *myGenShape;
}

const SymmetryGroup *
StructureBuild::getSymmetryGroup() const
{
  return mySymmetryGroup.get();
}

void
StructureBuild::setSymmetryGroup(SymmetryGroupPtr symGroup)
{
  mySymmetryGroup = symGroup;
}

StructureBuild::FixedSet
StructureBuild::getFixedSet() const
{
  FixedSet fixedSet;

  BOOST_FOREACH(AtomInfoMap::const_reference atomInfo, myAtomsInfo)
  {
    if(atomInfo.second->isFixed())
      fixedSet.insert(atomInfo.first->getIndex());
  }

  return fixedSet;
}

bool
StructureBuild::separateAtoms()
{
  SeparationData sepData(myStructure);
  sepData.separations = generateSepSqMatrix();
  const bool succeeded = myPointSeparator.separatePoints(&sepData);
  if(succeeded)
    myStructure.setAtomPositions(sepData.points);
  return succeeded;
}

void
StructureBuild::pushTransform(const ::arma::mat44 & transform)
{
  myTransformStack.push_back(transform);
  myTransformCurrent = false;
}

void
StructureBuild::popTransform()
{
  SSLIB_ASSERT(!myTransformStack.empty());

  myTransformStack.pop_back();
  myTransformCurrent = false;
}

const ::arma::mat44 &
StructureBuild::getTransform() const
{
  if(!myTransformCurrent)
  {
    // Do it this way to stop accumulation of error from lots of push/pops
    myTransform.eye();
    for(size_t i = 0; i < myTransformStack.size(); ++i)
      myTransform *= myTransformStack[i];
    myTransformCurrent = true;
  }
  return myTransform;
}

void
StructureBuild::pushSpeciesPairDistances(const SpeciesPairDistances & distances)
{
  mySpeciesPairDistancesStack.push_back(distances);
  if(!distances.empty())
    mySpeciesPairDistancesCurrent = false;
}

void
StructureBuild::popSpeciesPairDistances()
{
  const bool wasEmpty = mySpeciesPairDistances.empty();
  mySpeciesPairDistancesStack.pop_back();
  if(!wasEmpty)
    mySpeciesPairDistancesCurrent = false;
}
const StructureBuild::SpeciesPairDistances &
StructureBuild::getSpeciesPairDistances() const
{

  if(!mySpeciesPairDistancesCurrent)
  {
    SpeciesPairDistances::const_iterator it;
    for(int i = mySpeciesPairDistancesStack.size() - 1; i >= 0; --i)
    {
      BOOST_FOREACH(SpeciesPairDistances::const_reference pairDist, mySpeciesPairDistancesStack[i])
      {
        it = mySpeciesPairDistances.find(pairDist.first);
        if(it == mySpeciesPairDistances.end())
          mySpeciesPairDistances.insert(pairDist);
      }
    }
    mySpeciesPairDistancesCurrent = true;
  }
  return mySpeciesPairDistances;
}

::arma::mat
StructureBuild::generateSepSqMatrix() const
{
  const size_t numAtoms = myStructure.getNumAtoms();
  ::arma::mat sepSq = ::arma::zeros(numAtoms, numAtoms);

  SpeciesPair pair;
  for(size_t i = 0; i < numAtoms; ++i)
  {
    const common::Atom & atomI = myStructure.getAtom(i);
    const BuildAtomInfo * infoI = getAtomInfo(atomI);
    for(size_t j = i; j < numAtoms; ++j)
    {
      const common::Atom & atomJ = myStructure.getAtom(j);
      const BuildAtomInfo * infoJ = getAtomInfo(atomJ);
      pair.set(atomI.getSpecies(), atomJ.getSpecies());

      OptionalDouble rI, rJ;

      // Try to set the radii from build info (the most specific knowledge we have)
      if(infoI)
        rI = infoI->radius;
      if(infoJ)
        rJ = infoJ->radius;

      // If we couldn't find radii then try pair separation distances
      if(!rI && !rJ)
      {
        // Try doing it by pair distances
        SpeciesPairDistances::const_iterator it =
            getSpeciesPairDistances().find(pair);
        if(it != mySpeciesPairDistances.end())
        {
          // Give them each a half of the separation distance
          rI.reset(0.5 * it->second);
          rJ.reset(0.5 * it->second);
        }
      }

      rI = mySpeciesDb.getSpeciesPairDistance(pair);
      if(rI)
      {
        rJ.reset(0.5 * *rI);
        *rI = 0.5 * *rI;
      }

      // Finally if we still don't have radii try getting it from the species database
      // Try setting it from the species database radii
      if(!rI)
        rI = mySpeciesDb.getRadius(atomI.getSpecies());
      if(!rJ)
        rJ = mySpeciesDb.getRadius(atomJ.getSpecies());

      double separation = 0.0;
      if(rI)
        separation += *rI;
      if(rJ)
        separation += *rJ;
      sepSq(i, j) = separation * separation;
    }
  }

  return (1.0 - myAtomsOverlap) * ::arma::symmatu(sepSq);
}

}
}
