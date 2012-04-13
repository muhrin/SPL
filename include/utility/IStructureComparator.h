/*
 * IStructureComparator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_COMPARATOR_H
#define I_STRUCTURE_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx { namespace common {
	class Structure;
}}

namespace sstbx { namespace utility {

template <class ComparisonData>
class IStructureComparator
{
public:

	virtual ~IStructureComparator() {}

	virtual double compareStructures(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const = 0;

	virtual double compareStructures(
		const ComparisonData & str1Data,
		const ComparisonData & str2Data) const = 0;

	virtual bool areSimilar(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const = 0;

	virtual bool areSimilar(
		const ComparisonData & str1Data,
		const ComparisonData & str2Data) const = 0;

	virtual const ComparisonData * generateComparisonData(const ::sstbx::common::Structure & str) const = 0;
};

}}

#endif /* I_STRUCTURE_COMPARATOR_H */
