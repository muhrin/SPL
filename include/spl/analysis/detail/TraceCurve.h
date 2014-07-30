/*
 * TraceCurve.h
 *
 *  Created on: April 23, 2014
 *      Author: Martin Uhrin
 */

#ifndef TRACE_CURVE_DETAIL_H
#define TRACE_CURVE_DETAIL_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <boost/optional.hpp>

#include "spl/analysis/BezierCurve.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename VD>
  class TraceCurve< VD>::Vertex
  {
    typedef typename Kernel::FT FT;
    typedef BezierCurve< Kernel> Bezier;
  public:
    struct Type
    {
      enum Value
      {
        CORNER, CURVE
      };
    };
    Vertex() :
        myType(Type::CORNER)
    {
    }
    explicit
    Vertex(const Point & point) :
        myPoint(point), myType(Type::CORNER)
    {

    }

    const Point &
    point() const
    {
      return myPoint;
    }
    Point &
    point()
    {
      return myPoint;
    }

    const boost::optional< Bezier>
    getBezier() const
    {
      return myBezier;
    }
    void
    setBezier(const boost::optional< Bezier> & bezier)
    {
      myBezier = bezier;
      if(myBezier)
        myType = Type::CURVE;
    }

    typename Type::Value
    type() const
    {
      return myType;
    }

    // TEMP HACK
    Point end;

  private:
    Point myPoint;
    typename Type::Value myType;
    boost::optional< Bezier> myBezier;
  };

template< typename VD>
  void
  TraceCurve< VD>::pushBack(const Point & point)
  {
    myVertices.push_back(Vertex(point));
  }

template< typename VD>
  size_t
  TraceCurve< VD>::numVertices() const
  {
    return myVertices.size();
  }

template< typename VD>
  const typename TraceCurve< VD>::Vertex &
  TraceCurve< VD>::vertexFront() const
  {
    return myVertices.front();
  }

template< typename VD>
  typename TraceCurve< VD>::Vertex &
  TraceCurve< VD>::vertexFront()
  {
    return myVertices.front();
  }

template< typename VD>
  const typename TraceCurve< VD>::Vertex &
  TraceCurve< VD>::vertexBack() const
  {
    return myVertices.back();
  }

template< typename VD>
  typename TraceCurve< VD>::Vertex &
  TraceCurve< VD>::vertexBack()
  {
    return myVertices.back();
  }

template< typename VD>
  const typename TraceCurve< VD>::Vertex &
  TraceCurve< VD>::vertex(const size_t i) const
  {
    return myVertices[i];
  }

template< typename VD>
  typename TraceCurve< VD>::Vertex &
  TraceCurve< VD>::vertex(const size_t i)
  {
    return myVertices[i];
  }

}
}

#endif /* SPL_USE_CGAL */
#endif /* TRACE_CURVE_DETAIL_H */
