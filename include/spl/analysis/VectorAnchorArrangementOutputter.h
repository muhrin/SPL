/*
 * VectorAnchorArrangementOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef VECTOR_ANCHOR_ARRANGEMENT_OUTPUTTER_H
#define VECTOR_ANCHOR_ARRANGEMENT_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <map>

#include <spl/analysis/AnchorArrangementOutputter.h>

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename LabelType>
  class VectorAnchorArrangementOutputter : public AnchorArrangementOutputter<
      LabelType>
  {
  typedef AnchorArrangement<LabelType> Arrangement;
  public:
    typedef typename AnchorArrangementOutputter< LabelType>::InfoMap InfoMap;

    virtual
    ~VectorAnchorArrangementOutputter()
    {
    }

    virtual bool
    outputArrangement(
        const AnchorArrangement< LabelType> & arrangement) const;
    virtual bool
    outputArrangement(const AnchorArrangement< LabelType> & arrangement,
        const InfoMap & labelInfo) const;
  };

}
}

#include "spl/analysis/detail/VectorAnchorArrangementOutputter.h"

#endif // SPL_WITH_CGAL
#endif /* VECTOR_ANCHOR_ARRANGEMENT_OUTPUTTER_H */
