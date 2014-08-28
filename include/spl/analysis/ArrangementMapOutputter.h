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

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename MapTraits>
  class ArrangementMapOutputter
  {
  public:
    typedef typename MapTraits::Arrangement Arrangement;
    typedef typename MapTraits::Label LabelType;

    virtual
    ~ArrangementMapOutputter()
    {
    }

    virtual void
    outputArrangement(const Arrangement & map) const = 0;

    virtual bool
    setSeedName(const std::string & seedName)
    {
      return false;
    }

    virtual bool
    setColourMap(const std::map< LabelType, int> & colourMap)
    {
      return false;
    }

    virtual bool
    setXLabel(const std::string & xLabel)
    {
      return false;
    }

    virtual bool
    setYLabel(const std::string & yLabel)
    {
      return false;
    }
  };

}
}

#endif // SPL_USE_CGAL
#endif /* ARRANGEMENT_MAP_OUTPUTTER_H */
