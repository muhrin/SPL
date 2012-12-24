/*
 * GenericBufferedComparator.h
 *
 * Can be used by any IStructureComparator that meets the following criteria:
 * - has typedef DataTyp visible to GenericBufferedComparator that defines the
 *   comparison data type
 * - has methods with signatures:
 *   ::std::auto_ptr<DataTyp> generateComparisonData(const Structure & structure) const;
 *
 *   bool areSimilar(const Structrue & str1, const DataTyp & str1Data,
 *     const Structure & str2, const DataTyp & str2Data) const;
 * 
 *  double compareStructures(const Structrue & str1, const DataTyp & str1Data,
 *     const Structure & str2, const DataTyp & str2Data) const;
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


#ifndef GENERIC_BUFFERED_COMPARATOR_DEATIL_H
#define GENERIC_BUFFERED_COMPARATOR_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include <map>
#include <memory>

namespace sstbx {
namespace utility {

template <class ComparatorTyp>
GenericBufferedComparator<ComparatorTyp>::GenericBufferedComparator(const ComparatorTyp & comparator):
myComparator(comparator),
myTotalData(0),
myLastHandleId(0)
{}

template <class ComparatorTyp>
double GenericBufferedComparator<ComparatorTyp>::compareStructures(
  const ComparisonDataHandle & str1,
	const ComparisonDataHandle & str2)
{
  return myComparator.compareStructures(getComparisonData(str1.getId()), getComparisonData(str2.getId()));
}

template <class ComparatorTyp>
bool GenericBufferedComparator<ComparatorTyp>::areSimilar(
  const ComparisonDataHandle & str1,
	const ComparisonDataHandle & str2)
{
  return myComparator.areSimilar(getComparisonData(str1.getId()), getComparisonData(str2.getId()));
}

template <class ComparatorTyp>
typename GenericBufferedComparator<ComparatorTyp>::ComparisonDataHandle
GenericBufferedComparator<ComparatorTyp>::generateComparisonData(
  const sstbx::common::Structure & structure)
{
  HandleId id = generateHandleId();
  ComparisonDataHandle handle(id, *this);
  myComparisonData.insert(id, myComparator.generateComparisonData(structure).release());
  return handle;
}


template <class ComparatorTyp>
const IStructureComparator & GenericBufferedComparator<ComparatorTyp>::getComparator() const
{
  return myComparator;
}

template <class ComparatorTyp>
const typename GenericBufferedComparator<ComparatorTyp>::DataTyp &
GenericBufferedComparator<ComparatorTyp>::getComparisonData(const HandleId & id)
{
  const typename DataMap::const_iterator it = myComparisonData.find(id);

  SSLIB_ASSERT_MSG(it != myComparisonData.end(), "Comparison data could not be found.");

  return *it->second;
}

template <class ComparatorTyp>
void GenericBufferedComparator<ComparatorTyp>::handleReleased(const HandleId & id)
{
  const typename DataMap::iterator it = myComparisonData.find(id);

  SSLIB_ASSERT_MSG(it != myComparisonData.end(), "Comparison data could not be found.");

  myComparisonData.erase(it);
}

template <class ComparatorTyp>
typename GenericBufferedComparator<ComparatorTyp>::HandleId
GenericBufferedComparator<ComparatorTyp>::generateHandleId()
{
  return ++myLastHandleId;
}

}
}

#endif /* GENERIC_BUFFERED_COMPARATOR_H */
