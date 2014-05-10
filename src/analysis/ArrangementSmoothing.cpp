/*
 * ArrangementSmoothing.cpp
 *
 *  Created on: nov 4, 2013
 *      Author: Martin Uhrin
 */

#include "spl/analysis/ArrangementSmoothing.h"

#ifdef SPL_WITH_CGAL


namespace spl {
namespace analysis {

const double SmoothingOptions::SMOOTHING_FORCE_DEFAULT = 1.0;
const double SmoothingOptions::AREA_FORCE_DEFAULT = 100.0;
const double SmoothingOptions::VERTEX_FORCE_DEFAULT = 400.0;
const int SmoothingOptions::MAX_STEPS_DEFAULT = 10000;
const double SmoothingOptions::FORCE_TOL_DEFAULT = 1e-3;

}
}

#endif // SPL_WITH_CGAL
