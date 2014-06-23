/*
 * ArrangementMapOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef ARRANGEMENT_MAP_OUTPUTTER_H
#define ARRANGEMENT_MAP_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <map>
#include <ostream>

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename MapTraits>
  class ArrangementMapOutputter
  {
  public:
    typedef typename MapTraits::Arrangement Arrangement;
    typedef std::map< typename MapTraits::Label, std::string> LabelNames;

    virtual
    ~ArrangementMapOutputter()
    {
    }

    virtual void
    outputArrangement(const Arrangement & map,
        std::ostream * const os) const = 0;
    virtual void
    outputArrangement(const Arrangement & map, const LabelNames & labelNames,
        std::ostream * const os) const = 0;
    virtual std::string
    fileExtension() const = 0;
  };

}
}

#endif // SPL_WITH_CGAL
#endif /* ARRANGEMENT_MAP_OUTPUTTER_H */
