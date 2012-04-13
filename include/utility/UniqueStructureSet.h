/*
 * UniqueStructureSet.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef UNIQUE_STRUCTURE_SET_H
#define UNIQUE_STRUCTURE_SET_H

// INCLUDES /////////////////////////////////////////////
#include "IStructureSet.h"

#include "IStructureComparator.h"
#include "../common/Structure.h"

#include <boost/foreach.hpp>

#include <map>

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx { namespace utility {

template <class ComparisonData>
class UniqueStructureSet : public IStructureSet
{
private:

	struct StructureMetadata
	{
		StructureMetadata(const ComparisonData * const data = NULL);
		~StructureMetadata();

		/** The number of times this structure has been found */
		size_t				timesFound;

		const ComparisonData *	data;
	};

public:

	// TODO: Make StructureMetadata not a pointer!!!

	typedef std::map<sstbx::common::Structure *, StructureMetadata *> StructureSet;

	typedef std::pair<sstbx::common::Structure *, StructureMetadata *> StructureSetPair;

	typedef std::pair<sstbx::common::Structure *, bool> ReturnPair;

	UniqueStructureSet(const IStructureComparator<ComparisonData> & comparator);
	virtual ~UniqueStructureSet();

	// From IStructureSet //

	virtual const std::pair<sstbx::common::Structure *, bool>
		insert(sstbx::common::Structure * const str);

	virtual void clear();

	// End from IStructureSet //

private:

	const IStructureComparator<ComparisonData> & myComparator;

	StructureSet myStructures;
};


// IMPLEMENTATION ////////////////////////////////////////////


// StructureMetadata implementation ///////

template <class ComparisonData>
UniqueStructureSet<ComparisonData>::StructureMetadata::StructureMetadata(
	const ComparisonData * const inData):
data(inData),
timesFound(0)
{}

template <class ComparisonData>
UniqueStructureSet<ComparisonData>::StructureMetadata::~StructureMetadata()
{
	if(data)
		delete data;
}

// UniqueStructureSet implementation

template <class ComparisonData>
UniqueStructureSet<ComparisonData>::UniqueStructureSet(const IStructureComparator<ComparisonData> & comparator):
myComparator(comparator)
{}

template <class ComparisonData>
UniqueStructureSet<ComparisonData>::~UniqueStructureSet()
{
	for(typename StructureSet::const_iterator it = myStructures.begin(), end = myStructures.end();
		it != end; ++it)
	{
		delete it->second;
	}
	myStructures.clear();
}

template <class ComparisonData>
const std::pair<sstbx::common::Structure *, bool>
UniqueStructureSet<ComparisonData>::insert(sstbx::common::Structure * const str)
{
	// First creat the comparison data instance for this structure
	const ComparisonData * data = myComparator.generateComparisonData(*str);

	sstbx::common::Structure * similarTo = NULL;
	bool unique = true;
	for(typename StructureSet::const_iterator it = myStructures.begin(), end = myStructures.end();
		it != end; ++it)
	{
		if(myComparator.areSimilar(*data, *it->second->data))
		{
			++it->second->timesFound;
			similarTo = it->first;
			unique = false;
			break;
		}
	}

	// If it is not like any of those we have already then insert it
	if(unique)
	{
		myStructures.insert(
			StructureSetPair(
			str,
			new StructureMetadata(data)
			)
		);
	}

	return ReturnPair(unique ? str : similarTo, unique);
}

template <class ComparisonData>
void UniqueStructureSet<ComparisonData>::clear()
{
	BOOST_FOREACH(const StructureSetPair & p, myStructures)
	{
		delete p.second;
	}
	myStructures.clear();
}

}}


#endif /* UNIQUE_STRUCTURE_SET_H */
