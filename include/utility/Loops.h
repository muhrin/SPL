/*
 * Loops.h
 *
 * Arbitrarily nested loops counter.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef LOOPS_H
#define LOOPS_H

// INCLUDES /////////////////////////////////////////////
#include "../SSLib.h"

#include "MultiIdx.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace utility {

// TODO: Change this to use iterator model

template <typename CounterType = size_t>
class Loops
{
public:

	Loops(const MultiIdx<CounterType> & extents);

	size_t totalLoops() const;

	size_t globalPosition() const;

	/** Reset the loop counter back to the beginning */
	void reset();

	Loops & operator ++();	// Prefix ++ operator

	const MultiIdx<CounterType> & operator *() const;

	bool				isAtEnd() const;

private:

	const MultiIdx<CounterType>		myExtents;
	size_t							myTotalLoops;
	size_t							myGlobalPos;

	MultiIdx<CounterType>			myPosition;
	bool							myIsAtEnd;
};

// IMPLEMENTATION /////////////////////////////////////////////

template <typename CounterType>
Loops<CounterType>::Loops(const MultiIdx<CounterType> & extents):
myExtents(extents),
myPosition(extents.dims),
myGlobalPos(0)
{
	reset();

	// Calculate the total number of loops
	myTotalLoops = myExtents[0] != 0 ? myExtents[0] : 1;
	for(size_t i = 1; i < myExtents.dims; ++i)
	{
		myTotalLoops *= (myExtents[i] != 0 ? myExtents[i] : 1);
	}
}

template <typename CounterType>
size_t Loops<CounterType>::globalPosition() const
{
	return myGlobalPos;
}

template <typename CounterType>
void Loops<CounterType>::reset()
{
	myPosition.reset();
	myIsAtEnd = false;
	myGlobalPos = 0;
}

template <typename CounterType>
Loops<CounterType> & Loops<CounterType>::operator ++()
{
	bool increment = true;
	for(size_t i = 0; increment && i < myExtents.dims; ++i)
	{
		// Increment the position vector and check it does not exceed
		// the max number of loops
		if(++myPosition[i] >= myExtents[i])
		{
			myPosition[i] = 0;
		}
		else
		{
			// Successfully moved one position on in index space
			increment = false;
		}
	}
	if(increment)
		myIsAtEnd = true;
	else
		++myGlobalPos;

	return *this;
}

template <typename CounterType>
const MultiIdx<CounterType> & Loops<CounterType>::operator *() const
{
	return myPosition;
}

template <typename CounterType>
bool Loops<CounterType>::isAtEnd() const
{
	return myIsAtEnd;
}

}}

#endif /* LOOPS_H */
