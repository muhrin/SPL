/*
 * MultiIdx.h
 *
 * Array of runtime dimension.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MULTI_IDX_H
#define MULTI_IDX_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"
#include "utility/PromotableType.h"

#include <limits>
#include <stdexcept>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace utility {

template <typename IdxTyp = size_t>
class MultiIdx
{
public:
	explicit MultiIdx(const size_t _dims);
	MultiIdx(const size_t _dims, const IdxTyp initialVal);
	// Copy constructor
	MultiIdx(const MultiIdx<IdxTyp> & toCopy);
	~MultiIdx();

	/** Reset back to the origin */
	void reset();

	void fill(IdxTyp value);

	IdxTyp min() const;
	IdxTyp max() const;

	// Operators ///
	IdxTyp &		operator[](const size_t dim);
	const IdxTyp &	operator[](const size_t dim) const;

	// Same index type
	MultiIdx<IdxTyp> & operator=(const MultiIdx<IdxTyp> & rhs);
	MultiIdx<IdxTyp> operator+(const MultiIdx<IdxTyp> & rhs) const;
	MultiIdx<IdxTyp> operator-(const MultiIdx<IdxTyp> & rhs) const;
	MultiIdx<IdxTyp> & operator+=(const MultiIdx<IdxTyp> & rhs);
	MultiIdx<IdxTyp> & operator-=(const MultiIdx<IdxTyp> & rhs);
	bool operator==(const MultiIdx<IdxTyp> & rhs) const;
	MultiIdx<int> operator<(const MultiIdx<IdxTyp> & rhs) const;

	// Different index type
	template <typename RhsTyp>
	MultiIdx<IdxTyp> & operator=(const MultiIdx<RhsTyp> & rhs);
	template <typename OutTyp, typename RhsTyp>
	MultiIdx<OutTyp> operator-(const MultiIdx<RhsTyp> & rhs) const;
	template <typename RhsTyp>
	MultiIdx<IdxTyp> & operator+=(const MultiIdx<RhsTyp> & rhs);
	template <typename RhsTyp>
	MultiIdx<IdxTyp> & operator-=(const MultiIdx<RhsTyp> & rhs);
	template <typename RhsTyp>
	MultiIdx<int> operator<(const MultiIdx<RhsTyp> & rhs) const;

	template <typename LhsTyp, typename RhsTyp>
	friend MultiIdx<typename PromoteType<LhsTyp, RhsTyp>::result > operator+(
		const MultiIdx<LhsTyp> & lhs,
		const MultiIdx<RhsTyp> & rhs);

	template <typename LhsTyp, typename RhsTyp>
	friend MultiIdx<typename PromoteType<LhsTyp, RhsTyp>::result > operator-(
		const MultiIdx<LhsTyp> & lhs,
		const MultiIdx<RhsTyp> & rhs);

	// Overload unary operators

	template <typename RhsTyp>
    friend MultiIdx<RhsTyp> operator-(const MultiIdx<RhsTyp> & rhs);

	template <typename RhsTyp>
	friend ::std::ostream & operator <<(
		::std::ostream &			os,
		const MultiIdx<RhsTyp> &	rhs);

	const size_t dims;

private:
	/** The current multidimensional index */
	IdxTyp *	myIdx;
};

// IMPLEMENTATION ///////////////////////

template <typename IdxTyp>
MultiIdx<IdxTyp>::MultiIdx(const size_t _dims):
dims(_dims)
{
	myIdx = new IdxTyp[dims];
	reset();
}

template <typename IdxTyp>
MultiIdx<IdxTyp>::MultiIdx(const size_t _dims, const IdxTyp initialVal):
dims(_dims)
{
	myIdx = new IdxTyp[dims];
	fill(initialVal);
}

template <typename IdxTyp>
MultiIdx<IdxTyp>::MultiIdx(const MultiIdx & toCopy):
dims(toCopy.dims)
{
	myIdx = new IdxTyp[dims];
	memcpy(myIdx, toCopy.myIdx, sizeof(IdxTyp) * dims);
}

template <typename IdxTyp>
MultiIdx<IdxTyp>::~MultiIdx()
{
	if(myIdx)
		delete [] myIdx;
}

template <typename IdxTyp>
void MultiIdx<IdxTyp>::reset()
{
	memset(myIdx, 0, sizeof(IdxTyp) * dims);
}

template <typename IdxTyp>
void MultiIdx<IdxTyp>::fill(IdxTyp value)
{
	for(size_t i = 0; i < dims; ++i)
	{
		myIdx[i] = value;
	}
}

template <typename IdxTyp>
IdxTyp MultiIdx<IdxTyp>::min() const
{
	IdxTyp min = ::std::numeric_limits<IdxTyp>::max();
	for(size_t i = 0; i < dims; ++i)
	{
		min = ::std::min(min, myIdx[i]);
	}
	return min;
}

template <typename IdxTyp>
IdxTyp MultiIdx<IdxTyp>::max() const
{
	IdxTyp max = ::std::numeric_limits<IdxTyp>::min();
	for(size_t i = 0; i < dims; ++i)
	{
		max = ::std::max(max, myIdx[i]);
	}
	return max;
}

template <typename IdxTyp>
IdxTyp & MultiIdx<IdxTyp>::operator[](const size_t dim)
{
	SSE_ASSERT(dim < dims);
	return myIdx[dim];
}

template <typename IdxTyp>
const IdxTyp & MultiIdx<IdxTyp>::operator[](const size_t dim) const
{
	SSE_ASSERT(dim < dims);
	return myIdx[dim];
}

template <typename IdxTyp>
MultiIdx<IdxTyp> & MultiIdx<IdxTyp>::operator=(const MultiIdx<IdxTyp> & rhs)
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	memcpy(myIdx, rhs.myIdx, sizeof(IdxTyp) * dims);
	return *this;
}

template <typename IdxTyp>
MultiIdx<IdxTyp> MultiIdx<IdxTyp>::operator+(const MultiIdx<IdxTyp> & rhs) const
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<IdxTyp> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result[i] = myIdx[i] + rhs.myIdx[i];
	}
	return result;
}

template <typename IdxTyp>
MultiIdx<IdxTyp> MultiIdx<IdxTyp>::operator-(const MultiIdx<IdxTyp> & rhs) const
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<IdxTyp> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result[i] = myIdx[i] - rhs.myIdx[i];
	}
	return result;
}

template <typename IdxTyp>
MultiIdx<IdxTyp> & MultiIdx<IdxTyp>::operator+=(const MultiIdx<IdxTyp> & rhs)
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < dims; ++i)
	{
		myIdx[i] += rhs.myIdx[i];
	}
	return *this;
}

template <typename IdxTyp>
MultiIdx<IdxTyp> & MultiIdx<IdxTyp>::operator-=(const MultiIdx<IdxTyp> & rhs)
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < dims; ++i)
	{
		myIdx[i] -= rhs.myIdx[i];
	}
	return *this;
}

template <typename IdxTyp>
bool MultiIdx<IdxTyp>::operator==(const MultiIdx<IdxTyp> & rhs) const
{
	if(dims != rhs.dims)
		return false;

	bool result = true;
	for(size_t i = 0; i < dims; ++i)
	{
		if(myIdx[i] != rhs.myIdx[i])
		{
			result = false;
			break;
		}
	}
	return result;
}

template <typename IdxTyp>
MultiIdx<int>  MultiIdx<IdxTyp>::operator<(const MultiIdx<IdxTyp> & rhs) const
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<int> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result.myIdx[i] = myIdx[i] < rhs.myIdx[i];
	}
	return result;
}

template <typename IdxTyp>
template <typename RhsTyp>
MultiIdx<IdxTyp> & MultiIdx<IdxTyp>::operator=(const MultiIdx<RhsTyp> & rhs)
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < rhs.dims; ++i)
	{
		myIdx[i] = rhs[i];
	}
	return *this;
}

template <typename IdxTyp>
template <typename RhsTyp>
MultiIdx<IdxTyp> & MultiIdx<IdxTyp>::operator+=(const MultiIdx<RhsTyp> & rhs)
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < rhs.dims; ++i)
	{
		myIdx[i] += rhs[i];
	}
	return *this;
}

template <typename IdxTyp>
template <typename RhsTyp>
MultiIdx<IdxTyp> & MultiIdx<IdxTyp>::operator-=(const MultiIdx<RhsTyp> & rhs)
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < rhs.dims; ++i)
	{
		myIdx[i] -= rhs[i];
	}
	return *this;
}

template <typename IdxTyp>
template <typename RhsTyp>
MultiIdx<int> MultiIdx<IdxTyp>::operator<(const MultiIdx<RhsTyp> & rhs) const
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<int> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result.myIdx[i] = myIdx[i] < rhs[i];
	}
	return result;
}

template <typename IdxTyp>
template <typename OutTyp, typename RhsTyp>
MultiIdx<OutTyp>  MultiIdx<IdxTyp>::operator-(const MultiIdx<RhsTyp> & rhs) const
{
	if(dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<OutTyp> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result[i] = myIdx[i] - rhs[i];
	}
	return result;
}

template <typename RhsTyp>
MultiIdx<RhsTyp> operator-(const MultiIdx<RhsTyp> & rhs)
{
	MultiIdx<RhsTyp> negation(rhs.dims);
	for(size_t i = 0; i < rhs.dims; ++i)
	{
		negation.myIdx[i] = -rhs.myIdx[i];
	}
	return negation;
}

template <typename LhsTyp, typename RhsTyp>
inline MultiIdx<typename PromoteType<LhsTyp, RhsTyp>::result > operator+(
	const MultiIdx<LhsTyp> & lhs,
	const MultiIdx<RhsTyp> & rhs)
{
	typedef typename PromoteType<LhsTyp, RhsTyp>::result eOutT;

	if(lhs.dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	const size_t dims = lhs.dims;
	MultiIdx<eOutT> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result.myIdx[i] = lhs.myIdx[i] + rhs.myIdx[i];
	}
	return result;
}

template <typename LhsTyp, typename RhsTyp>
inline MultiIdx<typename PromoteType<LhsTyp, RhsTyp>::result > operator-(
	const MultiIdx<LhsTyp> & lhs,
	const MultiIdx<RhsTyp> & rhs)
{
	typedef typename PromoteType<LhsTyp, RhsTyp>::result eOutT;

	if(lhs.dims != rhs.dims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	const size_t dims = lhs.dims;
	MultiIdx<eOutT> result(dims);
	for(size_t i = 0; i < dims; ++i)
	{
		result.myIdx[i] = lhs.myIdx[i] - rhs.myIdx[i];
	}
	return result;
}

template <typename RhsTyp>
inline ::std::ostream & operator <<(
	::std::ostream &			os,
	const MultiIdx<RhsTyp> &	rhs)
{
	for(size_t i = 0; i < rhs.dims; ++i)
	{
		os << rhs.myIdx[i] << " ";
	}
	return os;
}

}}

#endif /* MULTI_IDX_H */

