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

#ifdef SPL_WITH_CGAL

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template <class MapTraits, class PointIterator>
typename MapTraits::Arrangement processPath(PointIterator first, PointIterator last);

}
}

#include "spl/analysis/detail/VoronoiPathTracer.h"

#endif /* SPL_WITH_CGAL */
#endif /* VORONOI_PATH_TRACER_H */
