/*
 * MatplotLibMapOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H
#define MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H

// INCLUDES ///////////////////
#include <boost/foreach.hpp>

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename MapTraits>
  class MatplotlibMapOutputter< MapTraits>::FacePath
  {
    typedef typename MapTraits::ArrTraits::Rat_point_2 Point;
    typedef typename Arrangement::X_monotone_curve_2 Curve;
  public:
    explicit
    FacePath(std::ostream * const os) :
        myOs(os)
    {
      *myOs << "face_data = [\n";
    }
    ~FacePath()
    {
      *myOs << INDENT << "]\n";
      *myOs << "face_codes, face_verts = zip(*face_data)\n";
      *myOs << "face_path = " << PATH << "(face_verts, face_codes)\n";
      *myOs << "face_patch = " << PATH_PATCH
          << "(face_path, facecolor='r', alpha=0.5)\n";
      *myOs << PLOT << ".add_patch(face_patch)\n";
    }

    void
    moveTo(const Point & p)
    {
      *myOs << INDENT << "(" << PATH << ".MOVETO, " << point(p) << "),\n";
    }
    void
    lineTo(const Point & p)
    {
      *myOs << INDENT << "(" << PATH << ".LINETO, " << point(p) << "),\n";
    }
    template< typename InputIterator>
      void
      curveTo(InputIterator first, InputIterator last)
      {
        for(InputIterator it = first; it != last; ++it)
          *myOs << INDENT << "(" << PATH << ".CURVE4, " << point(*it) << "),\n";
      }

  private:
    std::string
    point(const Point & p)
    {
      std::stringstream ss;
      ss << "(" << CGAL::to_double(p.x()) << ", " << CGAL::to_double(p.y())
          << ")";
      return ss.str();
    }

    Point myPos;
    std::ostream * const myOs;
  };

template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::PATH = "mpath.Path";
template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::PATH_PATCH =
      "mpatches.PathPatch";
template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::INDENT = "    ";
template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::PLOT = "ax";

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::outputArrangement(const Arrangement & map,
      std::ostream * const os) const
  {
    out(map, NULL, os);
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::outputArrangement(const Arrangement & map,
      const LabelNames & labelNames, std::ostream * const os) const
  {
    out(map, &labelNames, os);
  }

template< typename MapTraits>
  std::string
  MatplotlibMapOutputter< MapTraits>::fileExtension() const
  {
    return "py";
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::out(const Arrangement & map,
      const LabelNames * const names, std::ostream * const os) const
  {
    std::cout << map.number_of_faces() << std::endl;

    *os << "import matplotlib.path as mpath\n"
        << "import matplotlib.patches as mpatches\n"
        << "import matplotlib.pyplot as plt\n\n"
        << "fig, ax = plt.subplots()\n\n";

    BOOST_FOREACH(const Face & face,
        boost::make_iterator_range(map.faces_begin(), map.faces_end()))
    {
      drawFace(face, os);
      if(names && face.data().label)
      {
        const typename LabelNames::const_iterator it = names->find(
            *face.data().label);
//        drawLabel(it->second, pos, os);
      }
    }

    *os << "\nax.autoscale_view()\n";
    *os << "plt.show()\n";
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::drawFace(const Face & face,
      std::ostream * const os) const
  {
    if(!face.is_unbounded())
    {
      const typename Arrangement::Ccb_halfedge_const_circulator first =
          nextCurveHalfedge(face.outer_ccb());
      FacePath draw(os);
      draw.moveTo(first->curve().supporting_curve().control_point(0));

      typename Arrangement::Ccb_halfedge_const_circulator cl = first;
      do
      {
        drawCurveTo(cl->curve().supporting_curve(), &draw);
        cl = nextCurveHalfedge(cl);
      } while(cl != first);
    }
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::drawCurveTo(
      const typename MapTraits::ArrTraits::Curve_2 & curve,
      FacePath * const draw) const
  {
    const size_t nPoints = curve.number_of_control_points();
    if(nPoints == 2)
      draw->lineTo(curve.control_point(1));
    else
    {
      std::list< typename MapTraits::ArrTraits::Rat_point_2> pts;
      for(size_t i = 1; i < nPoints; ++i)
        pts.push_back(curve.control_point(i));
      draw->curveTo(pts.begin(), pts.end());
    }
  }

}
}

#endif /* MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H */
