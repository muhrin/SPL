/*
 * StructureBuild.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_BUILD_H
#define STRUCTURE_BUILD_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "spl/build_cell/BuildAtomInfo.h"
#include "spl/build_cell/GeneratorShape.h"
#include "spl/build_cell/PointSeparator.h"

namespace spl {
namespace common {
class Atom;
class AtomSpeciesDatabase;
class Structure;
}
namespace build_cell {
class BuildAtomInfo;
class StructureContents;
class SymmetryGroup;

class StructureBuild
{
  typedef std::map< const common::Atom *, BuildAtomInfo *> AtomInfoMap;
  typedef boost::ptr_vector< BuildAtomInfo> AtomInfoList;
public:
  typedef std::set< size_t> FixedSet;
  typedef UniquePtr< SymmetryGroup>::Type SymmetryGroupPtr;
  typedef AtomInfoList::iterator AtomInfoIterator;
  typedef UniquePtr< GeneratorShape>::Type GenShapePtr;
  typedef std::vector< arma::mat44> TransformStack;

  typedef std::map< SpeciesPair, double> SpeciesPairDistances;
  typedef std::vector< SpeciesPairDistances> SpeciesPairDistancesStack;

  class RadiusCalculator
  {
  public:
    static const double FALLBACK_RADIUS;

    RadiusCalculator(const double radius_ = 2.5,
        const bool isMultiplier_ = true);
    double
    getRadius(const double volume) const;

    double radius;
    bool isMultiplier;
  };

  class TransformPusher : boost::noncopyable
  {
  public:
    TransformPusher(StructureBuild & build, const arma::mat44 & trans) :
        myBuild(build)
    {
      myBuild.pushTransform(trans);
    }
    ~TransformPusher()
    {
      myBuild.popTransform();
    }
  private:
    StructureBuild & myBuild;
  };

  class SpeciesDistancesPusher : boost::noncopyable
  {
  public:
    SpeciesDistancesPusher(StructureBuild & build,
        const SpeciesPairDistances & dists) :
        myBuild(build)
    {
      myBuild.pushSpeciesPairDistances(dists);
    }
    ~SpeciesDistancesPusher()
    {
      myBuild.popSpeciesPairDistances();
    }
  private:
    StructureBuild & myBuild;
  };

  StructureBuild(common::Structure & structure,
      const StructureContents & intendedContents, const double atomsOverlap,
      const common::AtomSpeciesDatabase & speciesDb,
      const RadiusCalculator & radiusCalculator = RadiusCalculator());
  StructureBuild(common::Structure & structure,
      const StructureContents & intendedContents, const double atomsOverlap,
      const common::AtomSpeciesDatabase & speciesDb, GenShapePtr genShape);

  common::Structure &
  getStructure();
  const common::Structure &
  getStructure() const;

  BuildAtomInfo *
  getAtomInfo(common::Atom & atom);
  const BuildAtomInfo *
  getAtomInfo(const common::Atom & atom) const;
  BuildAtomInfo &
  createAtomInfo(common::Atom & atom);

  size_t
  getNumAtomInfos() const;
  AtomInfoIterator
  beginAtomInfo();
  AtomInfoIterator
  endAtomInfo();
  void
  addAtom(common::Atom & atom, BuildAtomInfo & atomInfo);
  void
  removeAtom(common::Atom & atom);

  const GeneratorShape &
  getGenShape() const;

  const SymmetryGroup *
  getSymmetryGroup() const;
  void
  setSymmetryGroup(SymmetryGroupPtr symGroup);

  FixedSet
  getFixedSet() const;

  bool
  separateAtoms();

  void
  pushTransform(const arma::mat44 & transform);
  void
  popTransform();
  const arma::mat44 &
  getTransform() const;

  void
  pushSpeciesPairDistances(const SpeciesPairDistances & distances);
  void
  popSpeciesPairDistances();
  const SpeciesPairDistances &
  getSpeciesPairDistances() const;

private:
  void
  atomInserted(BuildAtomInfo & atomInfo, common::Atom & atom);
  void
  atomRemoved(common::Atom & atom);
  arma::mat
  generateSepSqMatrix() const;

  common::Structure & myStructure;
  const StructureContents & myIntendedContents;
  const common::AtomSpeciesDatabase & mySpeciesDb;
  AtomInfoMap myAtomsInfo;
  AtomInfoList myAtomInfoList;
  SymmetryGroupPtr mySymmetryGroup;
  GenShapePtr myGenShape;

  const double myAtomsOverlap;

  TransformStack myTransformStack;
  mutable arma::mat44 myTransform;
  mutable bool myTransformCurrent;

  SpeciesPairDistancesStack mySpeciesPairDistancesStack;
  mutable SpeciesPairDistances mySpeciesPairDistances;
  mutable bool mySpeciesPairDistancesCurrent;

  const PointSeparator myPointSeparator;

  friend class BuildAtomInfo;
};

}
}

#endif /* STRUCTURE_BUILD_H */
