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
#include "SSLib.h"

#include "IStructureComparator.h"
#include "Loops.h"
#include "MultiArray.h"
#include "MultiIdx.h"

#include <boost/foreach.hpp>

#include <armadillo>

#include <algorithm>
#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace utility {

typedef ::std::pair<sstbx::utility::MultiIdx<size_t>, double> FinishedEdgePair;

template <typename CompDatTyp>
class EdgeMap
{
public:

	/**
	/* A pair of values that stores the dimension to edge detect in and the displacement
	/* from the current point in parameter space to the point to be edge detected against.
	/**/
	typedef ::std::pair<size_t, ::sstbx::utility::MultiIdx<int> > DimDirectionPair;

	struct EdgeData
	{
		typedef ::std::vector<DimDirectionPair > RemainingContainer;

		EdgeData();
		~EdgeData();

		void init(const size_t numDirections);

		/** The comparison data for this element */
		const CompDatTyp *					compData;
		/** The neighbours we still have to perform edge detection against */
		RemainingContainer					dimsToDo;
		/** The edge detection values along each parameter direction */
		::arma::Col<double>					directionValues;
		/** The final edge value (geometric average of direction values)*/
		double								value;
		/** The number of neighbours that still want to do edge detection against us. */
		size_t								references;
	};

	EdgeMap(
		const IStructureComparator<CompDatTyp> & comparator,
		const MultiIdx<size_t> & extents);
	~EdgeMap();


	/**
	/* Update the edge map with a structure at a given position in parameter space.
	/**/
	bool update(
		const ::sstbx::common::Structure &			structure,
		const MultiIdx<size_t> &					pos,
		::std::vector<FinishedEdgePair > * const	finishedEdges = NULL);


private:

	void generateMask();

	MultiIdx<size_t> externalToInternal(const MultiIdx<size_t> & external);
	MultiIdx<size_t> internalToExternal(const MultiIdx<size_t> & internal);

	int getMaskValue(const size_t dim, const MultiIdx<int> & dr) const;

	const IStructureComparator<CompDatTyp> &	myComparator;

	size_t										myExternalDims;
	/** The number of dimensions in parameter space that we are doing edge detection on. */
	size_t										myNDims;
	/**
	/* The members of this collection map to the dimensions in paramter space that
	/* we are creating the edge map for.
	/**/
	::std::vector<size_t>						myDimsMap;

	/** The convolution mask for each parameter space point */
	MultiArray<int>	*							myMask;
	MultiIdx<int> *								myMaskOrigin;

	MultiArray<EdgeData> *						myEdgeData;

	const size_t								myFilterLength;
	::arma::Col<int>							mySmoothing;
	::arma::Col<int>							myDerivative;
};

// IMPLEMENTATION ////////////////////////////


// EdgeData implementation //
template <typename CompDatTyp>
EdgeMap<CompDatTyp>::EdgeData::EdgeData():
compData(NULL),
value(0.0),
references(0)
{
}

template <typename CompDatTyp>
EdgeMap<CompDatTyp>::EdgeData::~EdgeData()
{
	if(compData)
		delete compData;
}

template <typename CompDatTyp>
void EdgeMap<CompDatTyp>::EdgeData::init(const size_t numDirections)
{
	directionValues.set_size(numDirections);
}

// EdgeMap implementation //

template <typename CompDatTyp>
EdgeMap<CompDatTyp>::EdgeMap(
	const IStructureComparator<CompDatTyp> & comparator,
	const MultiIdx<size_t> & _extents):
myComparator(comparator),
myFilterLength(3),
myMaskOrigin(NULL),
myMask(NULL),
myEdgeData(NULL)
{
	using ::sstbx::utility::MultiArray;
	using ::sstbx::utility::MultiIdx;

	// First create the map to parameter space dimensions we will do edge detection for
	// (i.e. all non-zero)
	myExternalDims = _extents.dims;
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
	mySmoothing << 1 << endr << 2 << endr << 1;
	myDerivative << -1 << endr << 0 << endr << 1;
	generateMask();

	// Copy over the number of steps for each dimension
	MultiIdx<size_t> reducedExtents(myNDims);
	for(size_t i = 0; i < myNDims; ++i)
	{
		reducedExtents[i] = _extents[myDimsMap[i]];
	}

	myEdgeData = new MultiArray<EdgeData>(reducedExtents);

	// Set the initial values for the edge data
	for(Loops<size_t> loops(reducedExtents); !loops.isAtEnd(); ++loops)
	{
		// Get the curent position in the sequence of loops
		const MultiIdx<size_t> & pos = *loops;

		// Get the edge
		EdgeData & edge = (*myEdgeData)[pos];
		edge.init(myNDims);

		// Now loop over the convolution mask
		for(Loops<size_t> convPos(myMask->getExtents()); !convPos.isAtEnd(); ++convPos)
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
					if(getMaskValue(i, relativePos) != 0)
					{
						edge.dimsToDo.push_back(DimDirectionPair(i, relativePos));
						++edge.references;
					}
				}
			}
		}
	}
}

template <typename CompDatTyp>
EdgeMap<CompDatTyp>::~EdgeMap()
{
	if(myMaskOrigin)
		delete myMaskOrigin;
	if(myMask)
		delete myMask;
	if(myEdgeData)
		delete myEdgeData;
}

template <typename CompDatTyp>
bool EdgeMap<CompDatTyp>::update(
	const ::sstbx::common::Structure &			str,
	const MultiIdx<size_t> &					pos,
	::std::vector<FinishedEdgePair > * const	finishedEdges)
{
	const MultiIdx<size_t> idx = externalToInternal(pos);

	// Get the edge
	EdgeData & edge = (*myEdgeData)[idx];

	// Generate the comparison data for this point
	edge.compData = myComparator.generateComparisonData(str);

	// Now do comparison with neighbours
	MultiIdx<size_t> neighPos(myNDims);

	// Keep track of whether we've finished any edges completely
	bool finishedEdge = false;

	for(typename EdgeData::RemainingContainer::iterator it = edge.dimsToDo.begin();
		it != edge.dimsToDo.end(); /* increment done in body */)
	{
		const DimDirectionPair & p = *it;
		neighPos = idx;
		neighPos += p.second;

		EdgeData & neighbour = (*myEdgeData)[neighPos];

		if(neighbour.compData)
		{
			DimDirectionPair reverse(p.first, -p.second);
			const double difference = getMaskValue(p.first, p.second) * myComparator.compareStructures(*edge.compData, *neighbour.compData);
			edge.directionValues(p.first) += difference;
			neighbour.directionValues(p.first) -= difference;

			typename EdgeData::RemainingContainer::iterator itNeigh = ::std::find(neighbour.dimsToDo.begin(), neighbour.dimsToDo.end(), reverse);
			if(itNeigh != neighbour.dimsToDo.end())
			{
				neighbour.dimsToDo.erase(itNeigh);
			}
			else
			{
				throw "Neighbour reference not found, the mask should be symmetric!";
			}

			// Drop the reference counts;
			--edge.references;
			--neighbour.references;

			it = edge.dimsToDo.erase(it);

			if(edge.references == 0 && edge.dimsToDo.empty())
			{
				delete edge.compData;
				edge.compData = NULL;
				edge.value	= ::std::sqrt(arma::dot(edge.directionValues, edge.directionValues));
				if(finishedEdges)
				{
					finishedEdges->push_back(FinishedEdgePair(pos, edge.value));
				}
				finishedEdge = true;
			}
			if(neighbour.references == 0 && neighbour.dimsToDo.empty())
			{
				delete neighbour.compData;
				neighbour.compData = NULL;
				neighbour.value	= ::std::sqrt(arma::dot(neighbour.directionValues, neighbour.directionValues));
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
	}


	return finishedEdge;
}

template <typename CompDatTyp>
void EdgeMap<CompDatTyp>::generateMask()
{
	using ::sstbx::utility::Loops;

	myMaskOrigin = new MultiIdx<int>(myNDims);
	myMaskOrigin->fill(1);

	// Create an extent for the convolution mask (size = 3^myNDims)
	const MultiIdx<size_t> maskExtents(myNDims, 3);

	// Generate the convolution mask along the 0th parameter direction
	myMask = new MultiArray<int>(maskExtents);
	myMask->fill(1);	// Fill with 1 so multiplication works

	for(Loops<size_t> loops(maskExtents); !loops.isAtEnd(); ++loops)
	{
		const MultiIdx<size_t> & pos = *loops;
		// Do 0th direction
		(*myMask)[pos] *= myDerivative(pos[0]);
		// Now do all other directions
		for(size_t i = 1; i < maskExtents.dims; ++i)
		{
			(*myMask)[pos] *= mySmoothing(pos[i]);
		}
	}
}

template <typename CompDatTyp>
MultiIdx<size_t> EdgeMap<CompDatTyp>::internalToExternal(const MultiIdx<size_t> & internal)
{
	MultiIdx<size_t> idx(myExternalDims, 0);
	for(size_t i = 0; i < myNDims; ++i)
	{
		idx[myDimsMap[i]] = internal[i];
	}
	return idx;
}

template <typename CompDatTyp>
MultiIdx<size_t> EdgeMap<CompDatTyp>::externalToInternal(const MultiIdx<size_t> & external)
{
	MultiIdx<size_t> idx(myNDims);
	for(size_t i = 0; i < myNDims; ++i)
	{
		idx[i] = external[myDimsMap[i]];
	}
	return idx;
}

template <typename CompDatTyp>
int EdgeMap<CompDatTyp>::getMaskValue(const size_t dim, const MultiIdx<int> & dr) const
{
	MultiIdx<int> relPos(dr.dims);
	relPos = dr;
	// Swap around 0th and dim^th dr coordinates to get correct direction
	::std::swap(relPos[0], relPos[dim]);
	// Now translate to mask coordinate space
	MultiIdx<size_t> pos(relPos.dims);
	pos = relPos + *myMaskOrigin;

	return (*myMask)[pos];
}

}}

#endif /* EDGE_MAP_H */
