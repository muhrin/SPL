/*
 * PromotableType.h
 *
 * Promotable type priorities
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef PROMOTABLE_TYPE_H
#define PROMOTABLE_TYPE_H

// INCLUDES /////////////////////////////////////////////
#include "../SSLib.h"

#include "MetaProgramming.h"

#include <complex>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace utility {

template <typename T1, typename T2>
struct IsPromotable
{
	static const bool value = false;
	// Default value T1 if no match is found
	typedef T1 result;
};

// All promotable definitions should subclass this as well
struct IsPromotableOk
{
	static const bool value = true;
};

template<typename T> struct IsPromotable<T,               T> : public IsPromotableOk { typedef T               result; };
template<typename T> struct IsPromotable<std::complex<T>, T> : public IsPromotableOk { typedef std::complex<T> result; };

template<> struct IsPromotable<std::complex<double>, std::complex<float> > : public IsPromotableOk { typedef std::complex<double> result; };
template<> struct IsPromotable<std::complex<double>, float>                : public IsPromotableOk { typedef std::complex<double> result; };
template<> struct IsPromotable<std::complex<float>,  double>               : public IsPromotableOk { typedef std::complex<double> result; };

template<typename T> struct IsPromotable<std::complex<T>, int> : public IsPromotableOk { typedef std::complex<T> result; };
template<typename T> struct IsPromotable<std::complex<T>, unsigned int> : public IsPromotableOk { typedef std::complex<T> result; };
template<typename T> struct IsPromotable<std::complex<T>, short> : public IsPromotableOk { typedef std::complex<T> result; };
template<typename T> struct IsPromotable<std::complex<T>, unsigned short> : public IsPromotableOk { typedef std::complex<T> result; };
template<typename T> struct IsPromotable<std::complex<T>, char>  : public IsPromotableOk { typedef std::complex<T> result; };
template<typename T> struct IsPromotable<std::complex<T>, unsigned char>  : public IsPromotableOk { typedef std::complex<T> result; };


template<> struct IsPromotable<double, float> : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, long  > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, unsigned long  > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, int  > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, unsigned int  > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, short  > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, unsigned short  > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, char   > : public IsPromotableOk { typedef double result; };
template<> struct IsPromotable<double, unsigned char   > : public IsPromotableOk { typedef double result; };

template<> struct IsPromotable<float, long> : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, unsigned long> : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, int> : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, unsigned int> : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, short> : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, unsigned short> : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, char > : public IsPromotableOk { typedef float result; };
template<> struct IsPromotable<float, unsigned char > : public IsPromotableOk { typedef float result; };

template<> struct IsPromotable<unsigned long, unsigned int> : public IsPromotableOk { typedef unsigned long result; };
template<> struct IsPromotable<unsigned long, unsigned short> : public IsPromotableOk { typedef unsigned long result; };
template<> struct IsPromotable<unsigned long, unsigned char > : public IsPromotableOk { typedef unsigned long result; };

template<> struct IsPromotable<long, unsigned long> : public IsPromotableOk { typedef long result; };  // float ?
template<> struct IsPromotable<long, unsigned int> : public IsPromotableOk { typedef long result; };
template<> struct IsPromotable<long, int> : public IsPromotableOk { typedef long result; };
template<> struct IsPromotable<long, short> : public IsPromotableOk { typedef long result; };
template<> struct IsPromotable<long, unsigned short> : public IsPromotableOk { typedef long result; };
template<> struct IsPromotable<long, char > : public IsPromotableOk { typedef long result; };
template<> struct IsPromotable<long, unsigned char > : public IsPromotableOk { typedef long result; };

template<> struct IsPromotable<int, unsigned int> : public IsPromotableOk { typedef int result; };  // float ?
template<> struct IsPromotable<int, short> : public IsPromotableOk { typedef int result; };
template<> struct IsPromotable<int, unsigned short> : public IsPromotableOk { typedef int result; };
template<> struct IsPromotable<int, char > : public IsPromotableOk { typedef int result; };
template<> struct IsPromotable<int, unsigned char > : public IsPromotableOk { typedef int result; };

template<> struct IsPromotable<unsigned int, short> : public IsPromotableOk { typedef int result; };  // float ?
template<> struct IsPromotable<unsigned int, unsigned short> : public IsPromotableOk { typedef unsigned int result; };
template<> struct IsPromotable<unsigned int, char > : public IsPromotableOk { typedef int result; };  // float ?
template<> struct IsPromotable<unsigned int, unsigned char > : public IsPromotableOk { typedef unsigned int result; };

template<> struct IsPromotable<short, unsigned short> : public IsPromotableOk { typedef short result; };  // int ?
template<> struct IsPromotable<short, char > : public IsPromotableOk { typedef short result; };
template<> struct IsPromotable<short, unsigned char > : public IsPromotableOk { typedef short result; };

template<> struct IsPromotable<unsigned short, char> : public IsPromotableOk { typedef short result; };  // int ?
template<> struct IsPromotable<unsigned short, unsigned char> : public IsPromotableOk { typedef unsigned short result; };

template<> struct IsPromotable<char, unsigned char> : public IsPromotableOk { typedef char result; };  // short ?


/**
/* Find the promote type for two arguments.  If it is not initially found
/* then try swapping the order of arguments.  If that fails then accept the default
/* as defined in IsPromotable.
/**/
template <typename T1, typename T2, bool TRY_SWAP = true>
struct PromoteType
{
	typedef typename If_<
		IsPromotable<T1, T2>::value,
		typename IsPromotable<T1, T2>::result,
		typename PromoteType<T2, T1, false>::result >::result
		result;
};

template <typename T1, typename T2>
struct PromoteType<T1, T2, false>
{
	typedef typename IsPromotable<T1, T2>::result result;
};

}}

#endif /* PROMOTABLE_TYPE_H */
