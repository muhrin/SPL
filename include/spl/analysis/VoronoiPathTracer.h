/*
 * VoronoiPathTracer.h
 *
 *  Created on: Feb 18, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_TRACER_H
#define VORONOI_PATH_TRACER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename MapTraits>
  struct VoronoiPathTracer
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef typename Kernel::FT FT;
    typedef typename Kernel::Point_2 Point;
    typedef std::pair< Point, typename MapTraits::Label> PointLabel;

    template< class PointIterator>
      static typename MapTraits::Arrangement
      processPath(PointIterator first, PointIterator last);

  private:
    // Not constructible
    VoronoiPathTracer()
    {
    }
  };

}
}

#include "spl/analysis/detail/VoronoiPathTracer.h"

#endif /* SPL_USE_CGAL */
#endif /* VORONOI_PATH_TRACER_H */
