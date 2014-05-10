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

#include <boost/ptr_container/ptr_map.hpp>

#include "spl/utility/IBufferedComparator.h"

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
}
namespace utility {

template< class ComparatorTyp>
  class GenericBufferedComparator : public IBufferedComparator
  {
    typedef IBufferedComparator::HandleId HandleId;
  public:

    // The comparator must have DataTyp defined
    typedef typename ComparatorTyp::DataTyp DataTyp;

    GenericBufferedComparator(const ComparatorTyp & comparator);
    virtual
    ~GenericBufferedComparator()
    {
    }

    virtual double
    compareStructures(const ComparisonDataHandle & str1,
        const ComparisonDataHandle & str2);

    virtual bool
    areSimilar(const ComparisonDataHandle & str1,
        const ComparisonDataHandle & str2);

    virtual ComparisonDataHandle
    generateComparisonData(const spl::common::Structure & structure);

    virtual const IStructureComparator &
    getComparator() const;

  private:

    typedef ::boost::ptr_map< HandleId, DataTyp> DataMap;

    const DataTyp &
    getComparisonData(const HandleId & id);
    void
    handleReleased(const HandleId & id);
    HandleId
    generateHandleId();

    HandleId myLastHandleId;
    const ComparatorTyp & myComparator;
    DataMap myComparisonData;
    size_t myTotalData;
  };

}
}

#include "detail/GenericBufferedComparator.h"

#endif /* GENERIC_BUFFERED_COMPARATOR_H */
