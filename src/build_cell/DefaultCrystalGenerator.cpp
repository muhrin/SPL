/*
 * CrystalGenerator.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "DefaultCrystalGenerator.h"

#include "AtomConstraintDescription.h"
#include "AtomGroupDescription.h"
#include "AtomsDescription.h"
#include "ICellGenerator.h"
#include "Minsep.h"
#include "StructureBuilder.h"
#include "StructureDescription.h"

#include "../common/AbstractFmidCell.h"
#include "../common/Atom.h"
#include "../common/Structure.h"
#include "../common/Utils.h"

namespace sstbx { namespace build_cell {

DefaultCrystalGenerator::DefaultCrystalGenerator(
		const StructureDescription &	structureDescription,
		const ICellGenerator<double> &	cellGenerator):
myStructureDescription(structureDescription),
myCellGenerator(cellGenerator),
maxAttempts(10000)
{
}

DefaultCrystalGenerator::~DefaultCrystalGenerator()
{
}




::sstbx::common::Structure * const DefaultCrystalGenerator::generateStructure() const
{
	using ::sstbx::common::AbstractFmidCell;
	using ::sstbx::common::Structure;

	// Create a new blank structure
	Structure * newStructure = new Structure();

	// Create a structure builder to generate the structure tree from the
	// description
	StructureBuilder builder(&myStructureDescription, newStructure);

	GenerationStatus outcome = NOT_STARTED;
	for(int i = 0; i < maxAttempts; ++i)
	{
		// Create a new unit cell
		AbstractFmidCell<double> * const cell = myCellGenerator.generateCell();

		newStructure->setUnitCell(cell);

		// Genetate atom positions
		outcome = generateAtomGroupPositions(builder, *newStructure);

		if(outcome == SUCCESS)
		{
			break;
		}
	}

	if(outcome != SUCCESS)
	{
		delete newStructure;
		newStructure = NULL;
	}

	return newStructure;
}

const StructureDescription & DefaultCrystalGenerator::getStructureDescription() const
{
	return myStructureDescription;
}

DefaultCrystalGenerator::GenerationStatus DefaultCrystalGenerator::generateAtomGroupPositions(
	const StructureBuilder & builder,
	::sstbx::common::AtomGroup & atomGroup)
	const
{
	using ::std::vector;
	using ::sstbx::common::AtomGroup;

	// First create atom positions for child groups
	const vector<AtomGroup *> & childGroups = atomGroup.getGroups();
	GenerationStatus outcome = SUCCESS;
	for(vector<AtomGroup *>::const_iterator it = childGroups.begin(),
		end = childGroups.end(); it != end; ++it)
	{
		outcome = generateAtomGroupPositions(builder, *(*it));

		// Check if it was possible to create the child group
		if(outcome != SUCCESS)
		{
			break;
		}
	}

	// If we were able to position the child groups then carry on
	if(outcome == SUCCESS)
	{
		bool valid = false;
		for(int i = 0; !valid && i < maxAttempts; ++i)
		{
			positionGroupsAndAtoms(builder, atomGroup);
			// Check if it satisfies the constraints
			valid = checkConstraints(builder, atomGroup);
		}
		if(!valid)
			outcome = FAILED_MAX_ATTEMPTS;
	}


	

	return outcome;
}

bool DefaultCrystalGenerator::positionGroupsAndAtoms(
	const StructureBuilder & builder,
	::sstbx::common::AtomGroup & atomGroup)
	const
{
	using ::std::vector;
	using ::sstbx::common::AbstractFmidCell;
	using ::sstbx::common::Atom;
	using ::sstbx::common::AtomGroup;

	const AbstractFmidCell<double> * cell = builder.getStructure()->getUnitCell();

	// Place the child groups
	const vector<AtomGroup *> & childGroups = atomGroup.getGroups();
	for(vector<AtomGroup *>::const_iterator it = childGroups.begin(),
		end = childGroups.end(); it != end; ++it)
	{
		AtomGroup * const childGroup = *it;
		childGroup->setPosition(cell->randomPoint());
	}

	// Now let's place the atoms
	const vector<Atom *> & atoms = atomGroup.getAtoms();
	for(vector<Atom *>::const_iterator it = atoms.begin(),
		end = atoms.end(); it != end; ++it)
	{
		Atom * const atom = *it;
		atom->setPosition(cell->randomPoint());
	}

	return true;
}

bool DefaultCrystalGenerator::checkConstraints(
	const StructureBuilder & builder,
	const ::sstbx::common::AtomGroup & atomGroup)
	const
{
	using ::std::vector;
	using ::sstbx::common::AbstractFmidCell;
	using ::sstbx::common::Atom;

	// Assume that each child group satisfies its constraints individually
	// so just check the atoms of this group
	const AbstractFmidCell<double> * const cell = builder.getStructure()->getUnitCell();

	Atom::Mat myAndDescendentAtoms;

	atomGroup.getAtomPositionsDescendent(myAndDescendentAtoms);

	bool constraintsSatisfied = true;
	double sepSq, minsep, minsepSq;
	const vector<Atom *> & atoms = atomGroup.getAtoms();
	for(size_t i = 0; constraintsSatisfied && i < atoms.size(); ++i)
	{
		const Atom * const atom = atoms[i];
		// Get the description for the atom which contains the constraints
		const AtomsDescription * desc = builder.getAtomsDescription(atom);

		const Minsep * const constraint = desc->getAtomConstraint<Minsep>(MINSEP);

		minsep = constraint->getMinsep();
		minsepSq = minsep * minsep;

		// Check that atom against all the others
		for(size_t j = 0; j < i; ++j)
		{
			sepSq = cell->getDistanceSqMinimumImg(myAndDescendentAtoms.col(i), myAndDescendentAtoms.col(j));
			if(sepSq < minsepSq)
			{
				constraintsSatisfied = false;
				break;
			}
		}
		for(size_t j = i + 1; constraintsSatisfied && j < myAndDescendentAtoms.n_cols; ++j)
		{
			sepSq = cell->getDistanceSqMinimumImg(myAndDescendentAtoms.col(i), myAndDescendentAtoms.col(j));
			if(sepSq < minsepSq)
			{
				constraintsSatisfied = false;
				break;
			}
		}
	}


	return constraintsSatisfied;
}

}}