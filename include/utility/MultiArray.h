/*
 * MultiArray.h
 *
 * Array of runtime dimension.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MULTI_ARRAY_H
#define MULTI_ARRAY_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"

#include "MultiIdx.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace utility {

template <typename Typ>
class MultiArray
{
public:

	MultiArray(const MultiIdx<size_t> & extents);
	~MultiArray();

	const MultiIdx<size_t> & getExtents() const;

	void fill(const Typ & value);

	// Accessors
	Typ & operator[](const MultiIdx<size_t> & idx);
	const Typ & operator[](const MultiIdx<size_t> & idx) const;

private:

	size_t	globalIdx(const MultiIdx<size_t> & dims) const;

	size_t					myTotal;
	const MultiIdx<size_t>	myExtents;
	Typ	*					myData;
	size_t *				myDimTotals;

};


// IMPLEMENTATION /////////////////////////////////////////////

template <typename Typ>
MultiArray<Typ>::MultiArray(const MultiIdx<size_t> & extents):
myExtents(extents),
myData(NULL),
myDimTotals(NULL)
{
	// Make sure we're being asked to create an array larger than 0!
	SSE_ASSERT(myExtents.dims > 0);

	myDimTotals = new size_t[myExtents.dims];

	// If the dimension is zero it contributes identity to the product
	myTotal	= myExtents[0] != 0 ? myExtents[0] : 1;
	// If the dimension is zero it contributes nothing to the sum
	myDimTotals[0]	= myExtents[0] != 0 ? 1 : 0;
	for(size_t i = 1; i < myExtents.dims; ++i)
	{
		// Save the total up to each dimension for later
		myDimTotals[i] = (myExtents[i] != 0 ? myTotal : 0);
		myTotal *= (myExtents[i] != 0 ? myExtents[i] : 1);
	}
	// Generate the linear memory for this multidimensional array
	myData = new Typ[myTotal];
	memset(myData, 0, sizeof(Typ) * myTotal);
}

template <typename Typ>
MultiArray<Typ>::~MultiArray()
{
	if(myDimTotals)
		delete myDimTotals;
	if(myData)
		delete [] myData;
}

template <typename Typ>
const MultiIdx<size_t> & MultiArray<Typ>::getExtents() const
{
	return myExtents;
}

template <typename Typ>
void MultiArray<Typ>::fill(const Typ & value)
{
	for(size_t i = 0; i < myTotal; ++i)
	{
		myData[i] = value;
	}
}

template <typename Typ>
Typ & MultiArray<Typ>::operator[](const MultiIdx<size_t> & idx)
{
	return myData[globalIdx(idx)];
}

template <typename Typ>
const Typ & MultiArray<Typ>::operator[](const MultiIdx<size_t> & idx) const
{
	return myData[globalIdx(idx)];
}

template <typename Typ>
size_t MultiArray<Typ>::globalIdx(const MultiIdx<size_t> & idx) const
{
	size_t global = 0;
	for(size_t i = 0; i < myExtents.dims; ++i)
	{
		global += myDimTotals[i] * idx[i];
	}
	return global;
}

}}

#endif /* MULTI_ARRAY_H */
