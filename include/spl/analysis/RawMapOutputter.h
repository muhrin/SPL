/*
 * RawMapOutputter.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Martin Uhrin
 */

#ifndef RAW_MAP_OUTPUTTER_H
#define RAW_MAP_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <map>
#include <ostream>

#include "spl/analysis/ArrangementMapOutputter.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename MapTraits>
  class RawMapOutputter : public ArrangementMapOutputter< MapTraits>
  {
  public:
    typedef typename MapTraits::Arrangement Arrangement;
    typedef typename MapTraits::Label LabelType;

    virtual
    ~RawMapOutputter()
    {
    }

    virtual void
    outputArrangement(const Arrangement & map) const;

  private:
    typedef typename MapTraits::Bezier Bezier;
    typedef typename Arrangement::Face Face;
    typedef typename MapTraits::ArrTraits::Point_2 Point;

    class PathDrawer;

    static std::string
    point(const Point & p)
    {
      std::stringstream ss;
      ss << "(" << CGAL::to_double(p.x()) << ", " << CGAL::to_double(p.y())
          << ")";
      return ss.str();
    }
  };

}
}

#include "spl/analysis/detail/RawMapOutputter.h"

#endif // SPL_USE_CGAL
#endif /* RAW_MAP_OUTPUTTER_H */
