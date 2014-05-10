/*
 * EdgeMap.h
 *
 * Arbitrarily nested loops counter.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef EDGE_MAP_H
#define EDGE_MAP_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include "spl/utility/IBufferedComparator.h"
#include "spl/utility/IStructureComparator.h"
#include "spl/utility/MultiArray.h"
#include "spl/utility/MultiIdx.h"
#include "spl/utility/MultiIdxRange.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <armadillo>

#include <algorithm>
#include <vector>
#include <spl/utility>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

typedef ::std::pair<spl::utility::MultiIdx<size_t>, double> FinishedEdgePair;

class EdgeMap
{
public:

	/**
	/* A pair of values that stores the dimension to edge detect in and the displacement
	/* from the current point in parameter space to the point to be edge detected against.
	/**/
	typedef ::std::pair<size_t, ::spl::utility::MultiIdx<int> > DimDirectionPair;

	class EdgeData
	{
  public:
		typedef ::std::vector<DimDirectionPair > RemainingContainer;

		EdgeData();

		void init(const size_t numDirections);

    void addNeighbour(
      const DimDirectionPair &  neigh,
      const double              maskVal);

		/** The comparison data handle for this element */
    IBufferedComparator::DataHandle compData;
		/** The neighbours we still have to perform edge detection against */
		RemainingContainer					dimsToDo;
		/** The edge detection values along each parameter direction */
		::arma::Col<double>					directionValues;
		/** The final edge value (geometric average of direction values)*/
		double								      value;
    /** The total of all the mask values that will be used for each dimension */
    arma::Col<double>           maskTotals;
		/** The number of neighbours that still want to do edge detection against us. */
		size_t								      references;
	};

	EdgeMap(
		const IStructureComparator & comparator,
		const MultiIdx<size_t> & extents);


	/**
	/* Update the edge map with a structure at a given position in parameter space.
	/**/
	bool update(
		const ::spl::common::Structure &			structure,
		const MultiIdx<size_t> &					pos,
		::std::vector<FinishedEdgePair > * const	finishedEdges = NULL);


private:

	void generateMask();

	MultiIdx<size_t> externalToInternal(const MultiIdx<size_t> & external);
	MultiIdx<size_t> internalToExternal(const MultiIdx<size_t> & internal);

	double getMaskValue(const size_t dim, const MultiIdx<int> & dr) const;

  const ::boost::shared_ptr<IBufferedComparator>	myComparator;

	size_t										myExternalDims;
	/** The number of dimensions in parameter space that we are doing edge detection on. */
	size_t										myNDims;
	/**
	/* The members of this collection map to the dimensions in paramter space that
	/* we are creating the edge map for.
	/**/
	::std::vector<size_t>					myDimsMap;

	/** The convolution mask for each parameter space point */
  ::boost::shared_ptr<MultiArray<double> >    myMask;
  ::boost::shared_ptr<MultiIdx<int> >					myMaskOrigin;

  ::boost::shared_ptr<MultiArray<EdgeData> >	myEdgeData;

	const size_t								  myFilterLength;
	::arma::Col<int>							mySmoothing;
	::arma::Col<int>							myDerivative;
};

// IMPLEMENTATION ////////////////////////////


// EdgeData implementation //
EdgeMap::EdgeData::EdgeData():
compData(NULL),
value(0.0),
references(0)
{
}

void EdgeMap::EdgeData::init(const size_t numDirections)
{
	directionValues.set_size(numDirections);
  directionValues.zeros();
  maskTotals.set_size(numDirections);
  maskTotals.zeros();
}

void EdgeMap::EdgeData::addNeighbour(
  const DimDirectionPair &  neigh,
  const double              maskVal)
{
  dimsToDo.push_back(neigh);
  maskTotals(neigh.first) += maskVal;
  ++references;
}

// EdgeMap implementation /////////////////

EdgeMap::EdgeMap(
	const IStructureComparator & comparator,
	const MultiIdx<size_t> & _extents):
myComparator(comparator.generateBuffered()),
myFilterLength(3)
{
	using ::spl::utility::MultiArray;
	using ::spl::utility::MultiIdx;

	// First create the map to parameter space dimensions we will do edge detection for
	// (i.e. all non-zero)
	myExternalDims = _extents.dims();
	for(size_t i = 0; i < myExternalDims; ++i)
	{
		if(_extents[i] != 0)
			myDimsMap.push_back(i);
	}
	myNDims = myDimsMap.size();

	// Set up the convolution mask
	using ::arma::endr;
	mySmoothing.set_size(myFilterLength);
	myDerivative.set_size(myFilterLength);
  // Smoothing mask, set central value to 1 and rest to zero for no smoothing
	mySmoothing << 0 << endr << 1 << endr << 0;
  // The 'central difference' method values, although we don't use nagative values
  // as structure differences can only positive
	myDerivative << 1 << endr << 0 << endr << 1;
	generateMask();

	// Copy over the number of steps for each dimension
	MultiIdx<size_t> reducedExtents(myNDims);
	for(size_t i = 0; i < myNDims; ++i)
	{
		reducedExtents[i] = _extents[myDimsMap[i]];
	}

	myEdgeData.reset(new MultiArray<EdgeData>(reducedExtents));

  const MultiIdxRange<size_t> extentsRange(MultiIdx<size_t>(reducedExtents.dims()), reducedExtents);

	// Set the initial values for the edge data
  BOOST_FOREACH(const MultiIdx<size_t> & pos, extentsRange)
	{
		// Get the edge
		EdgeData & edge = (*myEdgeData)[pos];
		edge.init(myNDims);

		// Now loop over the convolution mask
    const MultiIdxRange<size_t> maskRange(MultiIdx<size_t>(reducedExtents.dims()), myMask->getExtents());

    BOOST_FOREACH(const MultiIdx<size_t> & cPos, maskRange)
		{
			const MultiIdx<size_t> & cPos = *convPos;
			const MultiIdx<int> relativePos = cPos - *myMaskOrigin;
			//const MultiIdx<int> relativePos = *myMaskOrigin - cPos;

			const MultiIdx<int> globalPos = pos + relativePos;

			// Check that this doesn't go off the edge of the map
			if(globalPos.min() >= 0 && (globalPos < reducedExtents).min() >= 1)
			{
				// Now find all the non-zero mask values for this point in all directions
				for(size_t i = 0; i < myNDims; ++i)
				{
          const double maskVal = getMaskValue(i, relativePos);
					if(maskVal != 0)
					{
            edge.addNeighbour(DimDirectionPair(i, relativePos), maskVal);
					}
				}
			}
		}
	}
}

bool EdgeMap::update(
	const ::spl::common::Structure &			str,
	const MultiIdx<size_t> &					pos,
	::std::vector<FinishedEdgePair > * const	finishedEdges)
{
  using arma::dot;
  using std::sqrt;

	const MultiIdx<size_t> idx = externalToInternal(pos);

	// Get the edge
	EdgeData & edge = (*myEdgeData)[idx];

	// Generate the comparison data for this point
	edge.compData = myComparator->generateComparisonData(str);

	// Now do comparison with neighbours
	MultiIdx<size_t> neighPos(myNDims);

	// Keep track of whether we've finished any edge points completely
	bool finishedEdge = false;

  // To hold the result of the final directions value vector divided by the mask totals
  arma::Col<double> normValVector;
	for(EdgeData::RemainingContainer::iterator it = edge.dims()ToDo.begin();
		it != edge.dims()ToDo.end(); /* increment in body */)
	{
		const DimDirectionPair & p = *it;
		neighPos = idx + p.second;
		EdgeData & neighbour = (*myEdgeData)[neighPos];

    // Does the neighbour have comparison data yet?
		if(neighbour.compData)
		{
			DimDirectionPair reverse(p.first, -p.second);
      // Calculate the difference between the structures
			const double difference =
        getMaskValue(p.first, p.second) *
        myComparator->compareStructures(edge.compData, neighbour.compData);
			edge.directionValues(p.first)       += difference;
			neighbour.directionValues(p.first)  += difference;

      // Find ourselves amongst the neighbours neighbour list and remove
			EdgeData::RemainingContainer::iterator itNeigh = 
        std::find(neighbour.dims()ToDo.begin(), neighbour.dims()ToDo.end(), reverse);
			if(itNeigh != neighbour.dims()ToDo.end())
			{
				neighbour.dims()ToDo.erase(itNeigh);
			}
			else
			{
				throw "Neighbour reference not found, the mask should be symmetric!";
			}
      // Finished processing this neighbour so remove it from our neighbour list
			it = edge.dims()ToDo.erase(it);

			// Drop the reference counts;
			--edge.references;
			--neighbour.references;

			if(edge.references == 0 && edge.dims()ToDo.empty())
			{
        // Free up this comparison data, we won't be needing it anymore
        myComparator->releaseComparisonData(edge.compData);

        normValVector = edge.directionValues / edge.maskTotals;
				edge.value	= sqrt(dot(normValVector, normValVector) / myNDims);
				if(finishedEdges)
				{
					finishedEdges->push_back(FinishedEdgePair(pos, edge.value));
				}
				finishedEdge = true;
			}
			if(neighbour.references == 0 && neighbour.dims()ToDo.empty())
			{
        // Free up this comparison data, we won't be needing it anymore
        myComparator->releaseComparisonData(neighbour.compData);

        normValVector = neighbour.directionValues / neighbour.maskTotals;
				neighbour.value	= sqrt(dot(normValVector, normValVector) / myNDims);
				if(finishedEdges)
				{
					finishedEdges->push_back(FinishedEdgePair(internalToExternal(neighPos), neighbour.value));
				}
				finishedEdge = true;
			}
		}
		else
		{
			++it;
		}
	} // end for


	return finishedEdge;
}

void EdgeMap::generateMask()
{
	myMaskOrigin.reset(
    new MultiIdx<int>(myNDims, 1 /*This needs to be the center of filter*/)
  );

	// Create an extent for the convolution mask (size = 3^myNDims)
	const MultiIdx<size_t> maskExtents(myNDims, 3);

	// Generate the convolution mask along the 0th parameter direction
	myMask.reset(new MultiArray<double>(maskExtents));
	myMask->ones();	// Fill with 1 so multiplication works

  double maskVal = 0.0;
  const MultiIdxRange maskRange(MultiIdx<size_t>(maskExtents.dims()), maskExtents);

  BOOST_FOREACH(const MultiIdx<size_t> & pos, maskRange)
	{
    maskVal = 1.0;    // Make 
		// Do 0th direction
    maskVal *= myDerivative(pos[0]);
		// Now do all other directions
		for(size_t i = 1; i < maskExtents.dims(); ++i)
		{
      maskVal *= mySmoothing(pos[i]);
		}
    (*myMask)[pos] = maskVal;
	}
}

MultiIdx<size_t> EdgeMap::internalToExternal(const MultiIdx<size_t> & internal)
{
	MultiIdx<size_t> idx(myExternalDims, 0);
	for(size_t i = 0; i < myNDims; ++i)
	{
		idx[myDimsMap[i]] = internal[i];
	}
	return idx;
}

MultiIdx<size_t> EdgeMap::externalToInternal(const MultiIdx<size_t> & external)
{
	MultiIdx<size_t> idx(myNDims);
	for(size_t i = 0; i < myNDims; ++i)
	{
		idx[i] = external[myDimsMap[i]];
	}
	return idx;
}

double EdgeMap::getMaskValue(const size_t dim, const MultiIdx<int> & dr) const
{
	MultiIdx<int> relPos(dr.dims());
	relPos = dr;
	// Swap around 0th and dim^th dr coordinates to get correct direction
	::std::swap(relPos[0], relPos[dim]);
	// Now translate to mask coordinate space
	MultiIdx<size_t> pos(relPos.dims());
	pos = relPos + *myMaskOrigin;

	return (*myMask)[pos];
}

}}

#endif /* EDGE_MAP_H */
