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

#ifdef SPL_USE_CGAL

#include <map>
#include <ostream>

#include <boost/optional.hpp>

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename MapTraits>
  class ArrangementMapOutputter
  {
  public:
    typedef typename MapTraits::Arrangement Arrangement;

    struct LabelProperties
    {
      boost::optional< std::string> name;
      boost::optional< int> colour;
    };

    virtual
    ~ArrangementMapOutputter()
    {
    }

    virtual void
    outputArrangement(const Arrangement & map,
        std::ostream * const os) const = 0;
    virtual void
    outputArrangement(const Arrangement & map,
        const std::map< typename MapTraits::Label, LabelProperties> & labelsInfo,
        std::ostream * const os) const = 0;
    virtual std::string
    fileExtension() const = 0;
  };

}
}

#endif // SPL_USE_CGAL
#endif /* ARRANGEMENT_MAP_OUTPUTTER_H */
