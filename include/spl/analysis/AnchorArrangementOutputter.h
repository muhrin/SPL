/*
 * AnchorArrangementOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef ANCHOR_ARRANGEMENT_OUTPUTTER_H
#define ANCHOR_ARRANGEMENT_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <map>

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {
template< typename LabelType>
  class AnchorArrangement;

template< typename LabelType>
  class AnchorArrangementOutputter
  {
  public:
    typedef ::std::map< LabelType, ::std::string> InfoMap;

    virtual
    ~AnchorArrangementOutputter()
    {
    }

    virtual bool
    outputArrangement(
        const AnchorArrangement< LabelType> & arrangement) const = 0;
    virtual bool
    outputArrangement(const AnchorArrangement< LabelType> & arrangement,
        const InfoMap & labelInfo) const = 0;
  };

}
}

#endif // SPL_WITH_CGAL
#endif /* ANCHOR_ARRANGEMENT_OUTPUTTER_H */
