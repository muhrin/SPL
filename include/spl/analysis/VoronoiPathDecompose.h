/*
 * VoronoiPathDecompose.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_DECOMPOSE_H
#define VORONOI_PATH_DECOMPOSE_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <spl/analysis/VoronoiPathArrangement.h>

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename VD>
  void
  decomposePaths(const VD & voronoi,
      VoronoiPathArrangement< VD> * const arrangement);

}
}

#include "spl/analysis/detail/VoronoiPathDecompose.h"

#endif /* SPL_WITH_CGAL */
#endif /* VORONOI_PATH_DECOMPOSE_H */
