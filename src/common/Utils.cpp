/*
 * Utils.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: muhrin
 */

// INCLUDES //////////////////////////////////
#include "common/Utils.h"

#include <armadillo>

// NAMESPACES ////////////////////////////////
using namespace arma;

namespace sstbx { namespace common {

double randDouble()
{
	return ((double)rand() / RAND_MAX);
}

double randDouble(const double from, const double to)
{
	return ((double)rand() / RAND_MAX) * (to - from) + from;
}
//
//void randTriple(vec::fixed<3> & a)
//{
//	// Populate the vector
//	a[0] = randDouble();
//	a[1] = randDouble();
//	a[2] = randDouble();
//}
//
//
//vec * const randTriple(const double & from, const double & to)
//{
//	vec * const a = new vec::fixed<3>;
//	// Populate the vector
//	a[0] = randDouble(from, to);
//	a[1] = randDouble(from, to);
//	a[2] = randDouble(from, to);
//
//	return a;
//}
//
//void cctbxToArmaMat3(const scitbx::mat3<double> & toCopy, arma::Mat<double>::fixed<3, 3> & out)
//{
//	out.at(0,0) = toCopy.elems[0];
//	out.at(0,1) = toCopy.elems[1];
//	out.at(0,2) = toCopy.elems[2];
//	out.at(1,0) = toCopy.elems[3];
//	out.at(1,1) = toCopy.elems[4];
//	out.at(1,2) = toCopy.elems[5];
//	out.at(2,0) = toCopy.elems[6];
//	out.at(2,1) = toCopy.elems[7];
//	out.at(2,2) = toCopy.elems[8];
//}
//
//void cctbxVec3ToArma(const scitbx::vec3<double> & toCopy, arma::vec3 & out)
//{
//	out[0] = toCopy.elems[0];
//	out[1] = toCopy.elems[1];
//	out[2] = toCopy.elems[2];
//}
//
//void cctbxVec3ToArma(const scitbx::vec3<double> & toCopy, arma::Col<double> & out)
//{
//	out(0) = toCopy.elems[0];
//	out(0) = toCopy.elems[1];
//	out(0) = toCopy.elems[2];
//}
//
//void ccbtxFracVecToArma(const std::vector<cctbx::fractional<double> > & toCopy, arma::Mat<double> & out)
//{
//	for(size_t i = 0; i < toCopy.size(); ++i)
//	{
//		out(0, i) = toCopy[i].elems[0];
//		out(1, i) = toCopy[i].elems[1];
//		out(2, i) = toCopy[i].elems[2];
//	}
//}
//
//void armaMat3ToCctbx(const arma::Mat<double>::fixed<3, 3> & toCopy, scitbx::mat3<double> & out)
//{
//	out.elems[0] = out(0, 0);
//	out.elems[1] = out(1, 0);
//	out.elems[2] = out(2, 0);
//	out.elems[3] = out(0, 1);
//	out.elems[4] = out(1, 1);
//	out.elems[5] = out(2, 1);
//	out.elems[6] = out(0, 2);
//	out.elems[7] = out(1, 2);
//	out.elems[8] = out(2, 2);
//}

ProcessId getProcessId()
{
	return NS_BOOST_IPC_DETAIL::get_current_process_id();
}

}}
