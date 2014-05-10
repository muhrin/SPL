/*
 * MultiIdx.h
 *
 * An index in discreet n-space where n can be specified at runtime.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MULTI_IDX_H
#define MULTI_IDX_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <limits>
#include <stdexcept>

#include <boost/scoped_array.hpp>

#include "spl/SSLibAssert.h"
#include "spl/utility/PromotableType.h"


// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template <typename Integer = unsigned int>
class MultiIdx
{
public:

  MultiIdx(): myDims(0) {}
	explicit MultiIdx(const size_t _dims);
	MultiIdx(const size_t _dims, const Integer initialVal);
	// Copy constructor
	MultiIdx(const MultiIdx<Integer> & toCopy);

	/** Reset back to the origin */
	void reset();

	void fill(Integer value);

	Integer min() const;
	Integer max() const;
  Integer sum() const;
  Integer product() const;

	// Operators ///
	Integer &		operator[](const size_t dim);
	const Integer &	operator[](const size_t dim) const;

	// Same index type
	MultiIdx<Integer> & operator =(const MultiIdx<Integer> & rhs);
	MultiIdx<Integer> operator +(const MultiIdx<Integer> & rhs) const;
	MultiIdx<Integer> operator -(const MultiIdx<Integer> & rhs) const;
	MultiIdx<Integer> & operator +=(const MultiIdx<Integer> & rhs);
	MultiIdx<Integer> & operator -=(const MultiIdx<Integer> & rhs);
	bool operator ==(const MultiIdx<Integer> & rhs) const;
  bool operator !=(const MultiIdx<Integer> & rhs) const;
  bool operator <=(const MultiIdx<Integer> & rhs) const;
  bool operator >(const MultiIdx<Integer> & rhs) const;
	MultiIdx<int> operator<(const MultiIdx<Integer> & rhs) const;

	// Different index type
	template <typename RhsTyp>
	MultiIdx<Integer> & operator=(const MultiIdx<RhsTyp> & rhs);
	template <typename OutTyp, typename RhsTyp>
	MultiIdx<OutTyp> operator-(const MultiIdx<RhsTyp> & rhs) const;
	template <typename RhsTyp>
	MultiIdx<Integer> & operator+=(const MultiIdx<RhsTyp> & rhs);
	template <typename RhsTyp>
	MultiIdx<Integer> & operator-=(const MultiIdx<RhsTyp> & rhs);
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

	size_t dims() const;

  void setSize(const size_t newSize);

private:

  typedef ::boost::scoped_array<Integer> IdxPtr;

  bool resize(const size_t dims);

  /** The current number of dimensions */
  size_t    myDims;
	/** The current index */
	IdxPtr	  myIdx;
};

// IMPLEMENTATION ///////////////////////

template <typename Integer>
MultiIdx<Integer>::MultiIdx(const size_t dims):
myDims(dims)
{
  myIdx.reset(new Integer[myDims]);
	reset();
}

template <typename Integer>
MultiIdx<Integer>::MultiIdx(const size_t dims, const Integer initialVal):
myDims(dims)
{
	myIdx.reset(new Integer[myDims]);
	fill(initialVal);
}

template <typename Integer>
MultiIdx<Integer>::MultiIdx(const MultiIdx & toCopy):
myDims(toCopy.myDims)
{
	myIdx.reset(new Integer[myDims]);
  memcpy(myIdx.get(), toCopy.myIdx.get(), sizeof(Integer) * myDims);
}

template <typename Integer>
void MultiIdx<Integer>::reset()
{
	memset(myIdx.get(), 0, sizeof(Integer) * myDims);
}

template <typename Integer>
void MultiIdx<Integer>::fill(Integer value)
{
	for(size_t i = 0; i < myDims; ++i)
	{
		myIdx[i] = value;
	}
}

template <typename Integer>
Integer MultiIdx<Integer>::min() const
{
	Integer min = ::std::numeric_limits<Integer>::max();
	for(size_t i = 0; i < myDims; ++i)
	{
		min = ::std::min(min, myIdx[i]);
	}
	return min;
}

template <typename Integer>
Integer MultiIdx<Integer>::max() const
{
	Integer max = ::std::numeric_limits<Integer>::min();
	for(size_t i = 0; i < myDims; ++i)
	{
		max = ::std::max(max, myIdx[i]);
	}
	return max;
}

template <typename Integer>
Integer MultiIdx<Integer>::sum() const
{
	Integer sum = 0;
	for(size_t i = 0; i < myDims; ++i)
	{
		sum += myIdx[i];
	}
	return sum;
}

template <typename Integer>
Integer MultiIdx<Integer>::product() const
{
	Integer product = 1;
	for(size_t i = 0; i < myDims; ++i)
	{
		product *= myIdx[i];
	}
	return product;
}

template <typename Integer>
Integer & MultiIdx<Integer>::operator[](const size_t dim)
{
	SSLIB_ASSERT(dim < myDims);
	return myIdx[dim];
}

template <typename Integer>
const Integer & MultiIdx<Integer>::operator [](const size_t dim) const
{
	SSLIB_ASSERT(dim < myDims);
	return myIdx[dim];
}

template <typename Integer>
MultiIdx<Integer> & MultiIdx<Integer>::operator =(const MultiIdx<Integer> & rhs)
{
	if(myDims != rhs.myDims)
	{
    resize(rhs.dims());
	}

	memcpy(myIdx.get(), rhs.myIdx.get(), sizeof(Integer) * myDims);
	return *this;
}

template <typename Integer>
MultiIdx<Integer> MultiIdx<Integer>::operator+(const MultiIdx<Integer> & rhs) const
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<Integer> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
	{
		result[i] = myIdx[i] + rhs.myIdx[i];
	}
	return result;
}

template <typename Integer>
MultiIdx<Integer> MultiIdx<Integer>::operator-(const MultiIdx<Integer> & rhs) const
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<Integer> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
	{
		result[i] = myIdx[i] - rhs.myIdx[i];
	}
	return result;
}

template <typename Integer>
MultiIdx<Integer> & MultiIdx<Integer>::operator+=(const MultiIdx<Integer> & rhs)
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < myDims; ++i)
	{
		myIdx[i] += rhs.myIdx[i];
	}
	return *this;
}

template <typename Integer>
MultiIdx<Integer> & MultiIdx<Integer>::operator -=(const MultiIdx<Integer> & rhs)
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < myDims; ++i)
	{
		myIdx[i] -= rhs.myIdx[i];
	}
	return *this;
}

template <typename Integer>
bool MultiIdx<Integer>::operator ==(const MultiIdx<Integer> & rhs) const
{
	if(myDims != rhs.myDims)
		return false;

	bool result = true;
	for(size_t i = 0; i < myDims; ++i)
	{
		if(myIdx[i] != rhs.myIdx[i])
		{
			result = false;
			break;
		}
	}
	return result;
}

template <typename Integer>
bool MultiIdx<Integer>::operator !=(const MultiIdx<Integer> & rhs) const
{
	return !(*this == rhs);
}

template <typename Integer>
bool MultiIdx<Integer>::operator <=(const MultiIdx<Integer> & rhs) const
{
	if(myDims != rhs.myDims)
		return false;

	bool result = true;
	for(size_t i = 0; i < myDims; ++i)
	{
		if(myIdx[i] > rhs.myIdx[i])
		{
			result = false;
			break;
		}
	}
	return result;
}

template <typename Integer>
bool MultiIdx<Integer>::operator >(const MultiIdx<Integer> & rhs) const
{
	return !(*this <= rhs);
}

template <typename Integer>
MultiIdx<int>  MultiIdx<Integer>::operator<(const MultiIdx<Integer> & rhs) const
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<int> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
	{
		result.myIdx[i] = myIdx[i] < rhs.myIdx[i];
	}
	return result;
}

template <typename Integer>
template <typename RhsTyp>
MultiIdx<Integer> & MultiIdx<Integer>::operator=(const MultiIdx<RhsTyp> & rhs)
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < rhs.myDims; ++i)
	{
		myIdx[i] = rhs[i];
	}
	return *this;
}

template <typename Integer>
template <typename RhsTyp>
MultiIdx<Integer> & MultiIdx<Integer>::operator+=(const MultiIdx<RhsTyp> & rhs)
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < rhs.myDims; ++i)
	{
		myIdx[i] += rhs[i];
	}
	return *this;
}

template <typename Integer>
template <typename RhsTyp>
MultiIdx<Integer> & MultiIdx<Integer>::operator-=(const MultiIdx<RhsTyp> & rhs)
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	for(size_t i = 0; i < rhs.myDims; ++i)
	{
		myIdx[i] -= rhs[i];
	}
	return *this;
}

template <typename Integer>
template <typename RhsTyp>
MultiIdx<int> MultiIdx<Integer>::operator<(const MultiIdx<RhsTyp> & rhs) const
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<int> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
	{
		result.myIdx[i] = myIdx[i] < rhs[i];
	}
	return result;
}

template <typename Integer>
template <typename OutTyp, typename RhsTyp>
MultiIdx<OutTyp>  MultiIdx<Integer>::operator-(const MultiIdx<RhsTyp> & rhs) const
{
	if(myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	MultiIdx<OutTyp> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
	{
		result[i] = myIdx[i] - rhs[i];
	}
	return result;
}


template <typename Integer>
size_t MultiIdx<Integer>::dims() const
{
  return myDims;
}

template <typename Integer>
void MultiIdx<Integer>::setSize(const size_t newSize)
{
  if(newSize != myDims)
  {
    myDims = newSize;
    myIdx.reset(new Integer[myDims]);
  }
}

// LONE OPERATORS //////////////////////////////////////

template <typename RhsTyp>
MultiIdx<RhsTyp> operator-(const MultiIdx<RhsTyp> & rhs)
{
	MultiIdx<RhsTyp> negation(rhs.myDims);
	for(size_t i = 0; i < rhs.myDims; ++i)
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

	if(lhs.myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	const size_t myDims = lhs.myDims;
	MultiIdx<eOutT> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
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

	if(lhs.myDims != rhs.myDims)
	{
		throw ::std::logic_error("Multi index dimension mismatch");
	}
	const size_t myDims = lhs.myDims;
	MultiIdx<eOutT> result(myDims);
	for(size_t i = 0; i < myDims; ++i)
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
	for(size_t i = 0; i < rhs.myDims; ++i)
	{
		os << rhs.myIdx[i] << " ";
	}
	return os;
}

template <typename Integer>
bool MultiIdx<Integer>::resize(const size_t dims)
{
  if(myDims == dims)
    return false;

  myIdx.reset(new Integer[dims]);
  myDims = dims;

  return true;
}

}
}

#endif /* MULTI_IDX_H */

