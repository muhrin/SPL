/*
 * RawMapOutputter.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Martin Uhrin
 */

#ifndef RAW_MAP_OUTPUTTER_DETAIL_H
#define RAW_MAP_OUTPUTTER_DETAIL_H

// INCLUDES ///////////////////
#include <fstream>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <CGAL/centroid.h>
#include <CGAL/Polygon_2.h>

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename MapTraits>
  class RawMapOutputter< MapTraits>::PathDrawer
  {
  public:
    explicit
    PathDrawer(std::ostream * const os, const Point & startPt) :
        myOs(os)
    {
      *myOs << "MOVETO " << RawMapOutputter::point(startPt) << "\n";
    }

    void
    lineTo(const Point & p)
    {
      *myOs << "LINETO " << RawMapOutputter::point(p) << "\n";
    }

    void
    curveTo(const Bezier & bezier)
    {
      *myOs << "CURVE4 ";
      std::string points[3];
      for(size_t i = 1; i < 4; ++i)
        points[i - 1] = RawMapOutputter::point(bezier.control[i]);
      *myOs << boost::algorithm::join(points, ", ") << "\n";
    }

  private:
    std::ostream * const myOs;
  };

template< typename MapTraits>
  void
  RawMapOutputter< MapTraits>::outputArrangement(
      const RawMapOutputter< MapTraits>::Arrangement & map) const
  {
    const boost::iterator_range< typename Arrangement::Face_const_iterator> faces(
        map.faces_begin(), map.faces_end());

    std::ostream * const os = &std::cout;

    std::string colour, labelName;
    BOOST_FOREACH(const Face & face, faces)
    {
      if(!face.data().label)
        continue;

      *os << *face.data().label << "\nPoints\n";
      BOOST_FOREACH(const Point & pt, face.data().points)
        *os << point(pt) << "\n";

      if(!face.is_unbounded())
      {
        *os << "Path\n";
        const typename Arrangement::Ccb_halfedge_const_circulator first =
            face.outer_ccb();

        // Set the start point
        PathDrawer drawer(os, first->twin()->data().bezier ?
                first->target()->point() : first->source()->point());

        typename Arrangement::Ccb_halfedge_const_circulator cl = first;
        do
        {
          if(cl->data().bezier)
            drawer.curveTo(*cl->data().bezier);
          else if(cl->twin()->data().bezier)
          {
            Bezier bezier = *cl->twin()->data().bezier;
            bezier.reverse();
            drawer.curveTo(bezier);
          }
          else
            drawer.lineTo(cl->target()->point());

          ++cl;
        } while(cl != first);
      }
      *os << "\n\n";
    }
  }

}
}

#endif /* MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H */
