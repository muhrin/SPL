/*
 * MapArrangementTraits.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Martin Uhrin
 */

#ifndef MAP_ARRANGEMENT_H
#define MAP_ARRANGEMENT_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <utility>

#include <boost/optional.hpp>

#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>

#include "spl/analysis/BezierCurve.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType>
  class MapArrangementTraits
  {
  public:
    typedef LabelType Label;
    typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
    typedef BezierCurve< Kernel> Bezier;

    struct VertexInfo
    {
    };

    struct HalfedgeInfo
    {
      HalfedgeInfo() :
          label()
      {
      }
      boost::optional< Label> label;
      boost::optional< Bezier> bezier;
    };

    struct FaceInfo
    {
      boost::optional< Label> label;
    };

    // Arrangements stuff
    typedef CGAL::Arr_segment_traits_2<Kernel> ArrTraits;
    typedef typename ArrTraits::Point_2 ArrPoint;
    typedef CGAL::Arr_extended_dcel< ArrTraits, VertexInfo, HalfedgeInfo,
        FaceInfo> Dcel;
    typedef CGAL::Arrangement_2< ArrTraits, Dcel> Arrangement;
  };

}
}

#endif /* SPL_USE_CGAL */
#endif /* MAP_ARRANGEMENT_H */
