/*
 * StructureTriangulation.h
 *
 *  Created on: Jul 26, 2013
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_TRIANGULATION_H
#define STRUCTURE_TRIANGULATION_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <list>
#include <ostream>
#include <vector>

#include <CGAL/Cartesian.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Triangulation_geomview_ostream_2.h>
#include <CGAL/IO/Triangulation_geomview_ostream_3.h>


// FORWARD DECLARATIONS ///////


// DEFINITION ///////////////////////

namespace spl {
namespace common {
class Atom;
class Structure;
}

namespace analysis {

class StructureTriangulation
{
  //typedef CGAL::Cartesian<double> Kernel;
  typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
  //typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
public:
  typedef ::std::list<const common::Atom *> AtomsList;

  struct VertexInfo
  {
    VertexInfo()
    {
      atomIndex = -1;
      cellPosition[0] = cellPosition[1] = cellPosition[2] = 0;
    }
    bool isInOriginalCell() const
    { return cellPosition[0] == 0 && cellPosition[1] == 0 && cellPosition[2] == 0; }
    int atomIndex;
    int cellPosition[3];
  };

  typedef CGAL::Triangulation_vertex_base_with_info_3<VertexInfo, Kernel> Vertex;
  typedef CGAL::Triangulation_data_structure_3<Vertex> Triangulation;
  typedef CGAL::Delaunay_triangulation_3<Kernel, Triangulation> Delaunay;
  typedef Delaunay::Point Point;
  typedef CGAL::Polyhedron_3<Kernel> Polyhedron;

  StructureTriangulation(const common::Structure & structure);

  const Delaunay & getTriangulation() const;

  AtomsList getNeighbours(const common::Atom & atom) const;
  int getCoordination(const common::Atom & atom) const;
  Polyhedron getCoordinationPolyhedron(const common::Atom & atom) const;

  Delaunay::Vertex_handle findVertex(const common::Atom & atom) const;

  template <class Stream>
  void drawDual(Stream & os) const;

private:
  typedef ::std::vector<Delaunay::Vertex_handle> AtomVertexHandles;

  void buildTriangulation() const;
  void updateVertexHandles() const;

  const common::Structure & myStructure;
  // Acts as buffers of a sort that are lazily initialised so
  // to permit const functions to change them we make them mutable
  mutable Delaunay myTriangulation;
  mutable AtomVertexHandles myAtomVertices;
  mutable bool myTriangulationCurrent;
};

template <class Stream>
void StructureTriangulation::drawDual(Stream & os) const
{
  getTriangulation();
  myTriangulation.draw_dual(os);
  //os << myTriangulation;
}

}
}


#endif /* SPL_USE_CGAL */
#endif /* STRUCTURE_TRIANGULATION_H */
