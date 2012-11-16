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


#ifndef GENERIC_BUFFERED_COMPARATOR_H
#define GENERIC_BUFFERED_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////

#include <map>
#include <memory>

#include <boost/concept_check.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "utility/IBufferedComparator.h"

namespace sstbx {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
  class Structure;
}
namespace utility {

template <class ComparatorTyp>
class GenericBufferedComparator : public IBufferedComparator
{
public:

  // The comparator must have DataTyp defined
  typedef typename ComparatorTyp::DataTyp DataTyp;

  GenericBufferedComparator(const ComparatorTyp & comparator);
  virtual ~GenericBufferedComparator() {}

	virtual double compareStructures(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2);

	virtual bool areSimilar(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2);

  virtual bool hasComparisonDataFor(const common::Structure & str);
  virtual bool releaseComparisonDataFor(const common::Structure & str);

  virtual const IStructureComparator & getComparator() const;

private:

  typedef ::boost::ptr_map<const common::Structure *, DataTyp> DataMap;

  const DataTyp & getComparisonData(const common::Structure * structure);

  const ComparatorTyp &   myComparator;
  DataMap                 myComparisonData;
  size_t                  myTotalData;
};

// IMPLEMENTATION //////////////////////////////

template <class ComparatorTyp>
GenericBufferedComparator<ComparatorTyp>::GenericBufferedComparator(const ComparatorTyp & comparator):
myComparator(comparator),
myTotalData(0)
{}

template <class ComparatorTyp>
double GenericBufferedComparator<ComparatorTyp>::compareStructures(
  const sstbx::common::Structure & str1,
  const sstbx::common::Structure & str2)
{
  return myComparator.compareStructures(str1, getComparisonData(&str1), str2, getComparisonData(&str2));
}

template <class ComparatorTyp>
bool GenericBufferedComparator<ComparatorTyp>::areSimilar(
  const sstbx::common::Structure & str1,
  const sstbx::common::Structure & str2)
{
  return myComparator.areSimilar(str1, getComparisonData(&str1), str2, getComparisonData(&str2));
}

template <class ComparatorTyp>
bool GenericBufferedComparator<ComparatorTyp>::hasComparisonDataFor(const common::Structure & str)
{
  return myComparisonData.find(&str) != myComparisonData.end();
}

template <class ComparatorTyp>
bool GenericBufferedComparator<ComparatorTyp>::releaseComparisonDataFor(const common::Structure & str)
{
  return myComparisonData.erase(&str) > 0;
}

template <class ComparatorTyp>
const IStructureComparator & GenericBufferedComparator<ComparatorTyp>::getComparator() const
{
  return myComparator;
}

template <class ComparatorTyp>
const typename GenericBufferedComparator<ComparatorTyp>::DataTyp &
GenericBufferedComparator<ComparatorTyp>::getComparisonData(const common::Structure * structure)
{
  const ::std::auto_ptr<DataTyp> comparisonData;

  // Have we seen this structure before?
  typename DataMap::iterator it = myComparisonData.find(structure);
  if(it == myComparisonData.end())
  {
    it = myComparisonData.insert(structure, myComparator.generateComparisonData(*structure)).first;
  }

  return *it->second;
}

}
}

#endif /* GENERIC_BUFFERED_COMPARATOR_H */
