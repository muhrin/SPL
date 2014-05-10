/*
 * StructureBuilder.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_BUILDER_H
#define STRUCTURE_BUILDER_H

// INCLUDES /////////////////////////////////

#include "spl/SSLib.h"

#include <string>
#include <utility>

#include <boost/ptr_container/ptr_vector.hpp>

#include "spl/build_cell/AbsAtomsGenerator.h"
#include "spl/build_cell/IFragmentGenerator.h"
#include "spl/build_cell/IUnitCellGenerator.h"
#include "spl/build_cell/BuildCellFwd.h"
#include "spl/build_cell/PointGroups.h"
#include "spl/build_cell/StructureGenerator.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {
class AtomsDescription;

namespace detail {

class StructureBuilderCore
{
public:

  template< class T>
    void
    addGenerator(SSLIB_UNIQUE_PTR(T)generator);

  protected:
    typedef ::boost::ptr_vector< FragmentGenerator> Generators;

    Generators myGenerators;
  };

template< class T>
  void
  StructureBuilderCore::addGenerator(SSLIB_UNIQUE_PTR(T)generator)
  {
    myGenerators.push_back(generator.release());
  }

}
// namespace detail

      class StructureBuilder : public detail::StructureBuilderCore,
          public StructureGenerator,
          AbsAtomsGenerator
      {
      public:
        typedef AbsAtomsGenerator::const_iterator AtomsIterator;

        static const double DEFAULT_ATOMS_OVERLAP;
        static const bool DEFAULT_IS_CLUSTER;

        struct ConstructInfo
        {
          ConstructInfo()
          {
            atomsOverlap = DEFAULT_ATOMS_OVERLAP;
            isCluster = DEFAULT_IS_CLUSTER;
          }
          double atomsOverlap;
          bool isCluster;
        };

        StructureBuilder();
        StructureBuilder(const ConstructInfo & info);
        StructureBuilder(const StructureBuilder & toCopy);

        virtual GenerationOutcome
        generateStructure(common::StructurePtr & structureOut,
            const common::AtomSpeciesDatabase & speciesDb);
        virtual GenerationOutcome
        generateStructure(common::StructurePtr & structureOut,
            const common::AtomSpeciesDatabase & speciesDb,
            const GenerationSettings & info);

        virtual void
        setGenerationSettings(const GenerationSettings & settings);

        void
        setUnitCellGenerator(IUnitCellGeneratorPtr unitCellGenerator);
        const IUnitCellGenerator *
        getUnitCellGenerator() const;

        void
        setPointGroup(const PointGroup & pointGroup);
        const PointGroup &
        getPointGroup() const;

        bool
        isCluster() const;

        // Make public the things we want from AbsAtomsGenerator
        using AbsAtomsGenerator::insertAtoms;
        using AbsAtomsGenerator::numAtoms;
        using AbsAtomsGenerator::atomsBegin;
        using AbsAtomsGenerator::atomsEnd;

      private:
        bool
        chooseSymmetry(StructureBuild & build) const;
        GenerationOutcome
        generateSymmetry(StructureBuild & build) const;

        PointGroup myPointGroup;
        unsigned int myNumSymOps;
        const bool myIsCluster;
        const double myAtomsOverlap;

        IUnitCellGeneratorPtr myUnitCellGenerator;
      };

      }
      }

#endif /* STRUCTURE_BUILDER_H */
