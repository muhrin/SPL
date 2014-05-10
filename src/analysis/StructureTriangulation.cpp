/*
 * StructureTriangulation.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: Martin Uhrin
 */

#include "spl/analysis/StructureTriangulation.h"

#ifdef SPL_WITH_CGAL

#include <iterator>

#include <boost/foreach.hpp>

#include <armadillo>

#include "spl/common/Atom.h"
#include "spl/common/Structure.h"
#include "spl/common/UnitCell.h"
#include "spl/math/LinearAlgebra.h"

//#define DEBUG_STRUCTURE_TRIANGULATION

namespace spl {
namespace analysis {

StructureTriangulation::StructureTriangulation(const common::Structure & structure):
    myStructure(structure),
    myTriangulationCurrent(false)
{}

const StructureTriangulation::Delaunay & StructureTriangulation::getTriangulation() const
{
  if(!myTriangulationCurrent)
    buildTriangulation();

  return myTriangulation;
}

StructureTriangulation::AtomsList StructureTriangulation::getNeighbours(const common::Atom & atom) const
{
  // Make sure we have a valid triangulation
  getTriangulation();

  ::std::list<Delaunay::Vertex_handle> vertexHandles;
  getTriangulation().finite_adjacent_vertices(findVertex(atom), ::std::back_inserter(vertexHandles));

  AtomsList list;
  BOOST_FOREACH(Delaunay::Vertex_handle vh, vertexHandles)
  {
    if(vh->info().isInOriginalCell())
      list.push_back(&myStructure.getAtom(vh->info().atomIndex));
  }

  return list;
}

int StructureTriangulation::getCoordination(const common::Atom & atom) const
{
  // Make sure we have a valid triangulation
  getTriangulation();

  ::std::list<Delaunay::Vertex_handle> vertexHandles;
  getTriangulation().finite_adjacent_vertices(findVertex(atom), ::std::back_inserter(vertexHandles));
  return vertexHandles.size();
}

StructureTriangulation::Polyhedron StructureTriangulation::getCoordinationPolyhedron(const common::Atom & atom) const
{
  SSLIB_DIE_NOT_IMPLEMENTED();

  getTriangulation();

  Polyhedron poly;
  // TODO: Fill the polygon with the vertices adjacent to that of this atom
  ::std::list<Delaunay::Vertex_handle> vertexHandles;
  getTriangulation().finite_adjacent_vertices(findVertex(atom), ::std::back_inserter(vertexHandles));

  return poly;
}

StructureTriangulation::Delaunay::Vertex_handle
StructureTriangulation::findVertex(const common::Atom & atom) const
{
  getTriangulation();

  return myAtomVertices[atom.getIndex()];
}

void StructureTriangulation::buildTriangulation() const
{
  myTriangulation.clear();

  const common::UnitCell * cell = myStructure.getUnitCell();

  // Now insert the basis + images (if periodic) into the triangulation
  for(size_t atomIdx = 0; atomIdx < myStructure.getNumAtoms(); ++atomIdx)
  {
    const ::arma::vec3 & pos = myStructure.getAtom(atomIdx).getPosition();
    if(cell)
    {
      ::arma::vec3 iA, iAPlusJB;
      for(int i = -1; i <= 1; ++i)
      {
        iA = static_cast<double>(i) * cell->getAVec();
        for(int j = -1; j <= 1; ++j)
        {
          iAPlusJB = iA + static_cast<double>(j) * cell->getBVec();
          for(int k = -1; k <= 1; ++k)
          {
            const Delaunay::Vertex_handle vh =
              myTriangulation.insert(
                math::armaToCgal<Kernel>(pos + iAPlusJB + static_cast<double>(k) * cell->getCVec())
              );
            vh->info().atomIndex = atomIdx;
            vh->info().cellPosition[0] = i;
            vh->info().cellPosition[1] = j;
            vh->info().cellPosition[2] = k;
          }
        }
      }
    }
    else
    {
      Delaunay::Vertex_handle vh = myTriangulation.insert(math::armaToCgal<Kernel>(pos));
      vh->info().atomIndex = atomIdx;
    }
  }

  // Finally cache the atom vertex handles
  updateVertexHandles();
  myTriangulationCurrent = true;
}

void StructureTriangulation::updateVertexHandles() const
{
  myAtomVertices.resize(myStructure.getNumAtoms());

  // Update the buffer that maps atom indices to vertex handles
  for(Delaunay::Finite_vertices_iterator it = myTriangulation.finite_vertices_begin(),
      end = myTriangulation.finite_vertices_end(); it != end; ++it)
  {
    const VertexInfo & info = it->info();
    if(info.isInOriginalCell())
      myAtomVertices[info.atomIndex] = it;
  }
}

}
}

#endif // SPL_WITH_CGAL
