/*
 * GnuplotAnchorArrangementPlotter.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#ifndef GNUPLOT_ANCHOR_ARRANGEMENT_PLOTTER_H
#define GNUPLOT_ANCHOR_ARRANGEMENT_PLOTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <ostream>
#include <string>

#include "spl/analysis/AnchorArrangementOutputter.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template< typename LabelType>
  class GnuplotAnchorArrangementPlotter : public AnchorArrangementOutputter<
      LabelType>
  {
  typedef AnchorArrangement< LabelType> Arrangement;
  public:
    typedef typename AnchorArrangementOutputter< LabelType>::InfoMap InfoMap;

    GnuplotAnchorArrangementPlotter(const ::std::string & outputFileStem);
    virtual
    ~GnuplotAnchorArrangementPlotter()
    {
    }

    virtual bool
    outputArrangement(
        const Arrangement & arrangement) const;
    virtual bool
    outputArrangement(const Arrangement & arrangement,
        const InfoMap & labelInfo) const;

  private:
    void
    plotEdges(const Arrangement & arrangement, ::std::ostream * const os) const;
    ::std::string
    drawLabel(const double x, const double y, const ::std::string & label) const;

    const ::std::string myOutputFileStem;
  };



}
}

#include "spl/analysis/detail/GnuplotAnchorArrangementPlotter.h"

#endif // SPL_USE_CGAL
#endif /* GNUPLOT_ANCHOR_ARRANGEMENT_PLOTTER_H */
