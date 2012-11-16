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

double randDouble(const double from, const double to);

ProcessId getProcessId();

}}


#endif /* UTILS_H_ */
