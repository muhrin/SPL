/*
 * GnuplotConvexHullPlotter.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#ifndef GNUPLOT_CONVEX_HULL_PLOTTER_H
#define GNUPLOT_CONVEX_HULL_PLOTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <boost/filesystem/path.hpp>

#include "spl/analysis/ConvexHull.h"
#include "spl/analysis/ConvexHullOutputter.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

class GnuplotConvexHullPlotter : public ConvexHullOutputter
{
public:
  struct Settings
  {
    ::boost::filesystem::path outputDir;
  };

  GnuplotConvexHullPlotter();
  GnuplotConvexHullPlotter(const Settings & settings);

  virtual bool
  outputHull(const ConvexHull & convexHull) const;
  virtual bool
  outputHull(const ConvexHull & convexHull,
      const ConvexHullInfoSupplier & infoSupplier) const;

  bool
  getDrawTieLines() const;
  void
  setDrawTieLines(const bool draw);

  bool
  getSupressEnergyDimension() const;
  void
  setSupressEnergyDimension(const bool supress);

  void
  setDrawHullLabels(const bool label);

  void
  setDrawOffHullPoints(const bool draw);

private:
  static const double LABEL_MARGIN;
  static const int BOUNDARY_LINE_STYLE;
  static const int HULL_POINT_LINE_STYLE;
  static const int OFF_HULL_LINE_STYLE;

  class Plot
  {
  public:
    Plot();
    ::std::string
    drawLine(const ConvexHull::PointD & x0, const ConvexHull::PointD & x1);
    ::std::string
    drawLine(const ConvexHull::PointD & x0, const ConvexHull::PointD & x1,
        const int lineStyle);
    ::std::string
    drawLabel(const ::std::string label, const ConvexHull::PointD & x) const;
    ::std::string
    printPoint(const ConvexHull::PointD & point) const;

  private:
    int myArrowCounter;
    int myLabelCounter;
  };

  ::std::string
  printPoint(const ConvexHull::PointD & point) const;

  void
  setStyles(::std::ostream & os, const ConvexHull & convexHull) const;
  void
  drawBoundary(::std::ostream & os, const ConvexHull & convexHull,
      Plot & plot) const;
  void
  drawTieLines(::std::ostream & os, const ConvexHull & convexHull,
      Plot & plot) const;
  void
  drawEndpointLabels(::std::ostream & os, const ConvexHull & convexHull,
      Plot & plot) const;

  bool
  doOutputHull(const ConvexHull & convexHull,
      const ConvexHullInfoSupplier * const infoSupplier) const;
  int
  plotDims(const ConvexHull & convexHull) const;
  ConvexHull::PointD
  prepPoint(const ConvexHull::PointD & point) const;
  ::std::string
  generateHullFileStem(const ConvexHull & convexHull) const;

  const Settings mySettings;
  bool myDrawBoundary;
  bool myDrawTieLines;
  bool myDrawLabels;
  bool myDrawOffHullPoints;
  bool mySupressEnergyDimension;
};

}
}

#endif // SPL_USE_CGAL
#endif // GNUPLOT_CONVEX_HULL_PLOTTER_H
