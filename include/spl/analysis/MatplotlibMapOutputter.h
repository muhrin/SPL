/*
 * MatplotlibMapOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef MATPLOTLIB_MAP_OUTPUTTER_H
#define MATPLOTLIB_MAP_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include "spl/analysis/ArrangementMapOutputter.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename MapTraits>
  class MatplotlibMapOutputter : public ArrangementMapOutputter< MapTraits>
  {
  public:
    typedef typename MapTraits::Arrangement Arrangement;
    typedef std::map< typename MapTraits::Label, std::string> LabelNames;

    virtual
    ~MatplotlibMapOutputter()
    {
    }

    virtual void
    outputArrangement(const Arrangement & map, std::ostream * const os) const;
    virtual void
    outputArrangement(const Arrangement & map, const LabelNames & labelNames,
        std::ostream * const os) const;
    virtual std::string
    fileExtension() const;

  private:
    typedef typename Arrangement::Face Face;

    class FacePath;

    static const std::string PATH;
    static const std::string PATH_PATCH;
    static const std::string INDENT;
    static const std::string PLOT;

    void
    out(const Arrangement & map, const LabelNames * const names,
        std::ostream * const os) const;
    void
    drawFace(const Face & face, std::ostream * const os) const;
    void
    drawCurveTo(const typename MapTraits::ArrTraits::Curve_2 & curve,
        FacePath * const draw) const;

    template< typename HalfedgeCirculator>
    HalfedgeCirculator
    nextCurveHalfedge(const HalfedgeCirculator & start) const
    {
      const typename MapTraits::ArrTraits::Curve_2 & c = start->curve().supporting_curve();
      HalfedgeCirculator cl = start;
      ++cl;
      while(cl != start && cl->curve().supporting_curve().is_same(c))
      {
        ++cl;
      }
      return cl;
    }
  };

}
}

#include "spl/analysis/detail/MatplotlibMapOutputter.h"

#endif // SPL_WITH_CGAL
#endif /* MATPLOTLIB_MAP_OUTPUTTER_H */
