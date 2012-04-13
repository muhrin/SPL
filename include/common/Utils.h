/*
 * Utils.h
 *
 *  Created on: Aug 22, 2011
 *      Author: Martin Uhrin
 */


#ifndef UTILS_H_
#define UTILS_H_

// INCLUDES ///////////////////////////////////////
#include <armadillo>

#include <cctbx/uctbx.h>
//#include <scitbx/mat3.h>

// Tell boost not to try to link to the date_time library as interprocess
// only uses a portion that doesn't need compilation.
// See: http://www.boost.org/doc/libs/1_49_0/doc/html/interprocess.html#interprocess.intro.introduction_building_interprocess
#define BOOST_DATE_TIME_NO_LIB

#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/version.hpp>
#include <boost/any.hpp>

// FORWARD DECLARES ////////////////////////////////

// DEFINES ////////////////////////////////////////
#define EPSILON_REL 1e-5;

#if (BOOST_VERSION / 100000) <= 1 && ((BOOST_VERSION / 100) % 1000) <= 47
	#define	NS_BOOST_IPC_DETAIL boost::interprocess::detail
#else
	#define NS_BOOST_IPC_DETAIL boost::interprocess::ipcdetail
#endif

#define ProcessId NS_BOOST_IPC_DETAIL::OS_process_id_t

// FUNCTIONS ////////////////////////////////////////

namespace sstbx { namespace common {

double randDouble();

double randDouble(const double & from, const double & to);

////void randTriple(arma::Col<double> & a);
//void randTriple(arma::vec::fixed<3> & vec);
//
//arma::Col<double> * const randTriple(const double & from, const double & to);
//
//void cctbxVec3ToArma(const scitbx::vec3<double> & toCopy, arma::vec3 & out);
//
//void cctbxVec3ToArma(const scitbx::vec3<double> & toCopy, arma::Col<double> & out);
//
//void cctbxToArmaMat33(const scitbx::mat3<double> & toCopy, arma::Mat<double>::fixed<3, 3> & out);
//
//void ccbtxFracVecToArma(const std::vector<cctbx::fractional<double> > & toCopy, arma::Mat<double> & out);
//
//void armaMat3ToCctbx(const arma::Mat<double>::fixed<3, 3> & toCopy, scitbx::mat3<double> & out);

ProcessId getProcessId();

}}


#endif /* UTILS_H_ */