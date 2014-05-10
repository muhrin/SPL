/*
 * ArrangementSmoothing.h
 *
 *  Created on: Nov 4, 2013
 *      Author: Martin Uhrin
 */

#ifndef ARRANGEMENT_SMOOTHING_H
#define ARRANGEMENT_SMOOTHING_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL


// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType>
  class AnchorArrangement;

struct SmoothingOptions
{
  static const double SMOOTHING_FORCE_DEFAULT;
  static const double AREA_FORCE_DEFAULT;
  static const double VERTEX_FORCE_DEFAULT;
  static const int MAX_STEPS_DEFAULT;
  static const double FORCE_TOL_DEFAULT;

  SmoothingOptions():
    smoothingForceStrength(SMOOTHING_FORCE_DEFAULT),
    areaForceStrength(AREA_FORCE_DEFAULT),
    vertexForceStrength(VERTEX_FORCE_DEFAULT),
    maxSteps(MAX_STEPS_DEFAULT),
    forceTol(FORCE_TOL_DEFAULT)
  {
  }
  double smoothingForceStrength;
  double areaForceStrength;
  double vertexForceStrength;
  int maxSteps;
  double forceTol;
};

template < typename LabelType>
void
smoothArrangement(const SmoothingOptions & options, AnchorArrangement<LabelType> * const arr);

}
}

#include "spl/analysis/detail/ArrangementSmoothing.h"

#endif /* SPL_WITH_CGAL */
#endif /* ARRANGEMENT_SMOOTHING_H */
