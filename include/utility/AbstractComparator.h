/*
 * AbstractComparator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_COMPARATOR_H
#define ABSTRACT_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"

#include "utility/IStructureComparator.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx { namespace common {
	class Structure;
}}

namespace sstbx { namespace utility {

template <class ComparisonData>
class AbstractComparator : public virtual IStructureComparator<ComparisonData>
{
public:

	using IStructureComparator<ComparisonData>::compareStructures;
	using IStructureComparator<ComparisonData>::areSimilar;

	virtual double compareStructures(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const;

	virtual bool areSimilar(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const;

	const ComparisonData * generateComparisonData(const ::sstbx::common::Structure & str) const;
};

// IMPLEMENTATION ////////////////////////////////////////////

template <class ComparisonData>
double AbstractComparator<ComparisonData>::compareStructures(
	const sstbx::common::Structure & str1,
	const sstbx::common::Structure & str2) const
{
	const ComparisonData * data1 = generateComparisonData(str1);
	const ComparisonData * data2 = generateComparisonData(str1);

	const double outcome = compareStructures(*data1, *data2);

	// Clean up
	delete data1; data1 = NULL;
	delete data2; data2 = NULL;

	return outcome;
}

template <class ComparisonData>
bool AbstractComparator<ComparisonData>::areSimilar(
	const sstbx::common::Structure & str1,
	const sstbx::common::Structure & str2) const
{
	const ComparisonData * data1 = generateComparisonData(str1);
	const ComparisonData * data2 = generateComparisonData(str1);

	const bool outcome = areSimilar(*data1, *data2);

	// Clean up
	delete data1; data1 = NULL;
	delete data2; data2 = NULL;

	return outcome;
}


template <class ComparisonData>
const ComparisonData *
AbstractComparator<ComparisonData>::generateComparisonData(const ::sstbx::common::Structure & str) const
{
	return NULL;
}

}}

#endif /* ABSTRACT_COMPARATOR_H */
