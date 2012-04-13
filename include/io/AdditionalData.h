/*
 * AdditionalData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ADDITIONAL_DATA_H
#define ADDITIONAL_DATA_H

// INCLUDES /////////////////////////////////////////////

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx { namespace io {

struct AdditionalData
{
public:

	::boost::optional<double> enthalpy;
	::boost::optional<std::string> name;
	::boost::optional<double> pressure;
	::boost::optional<std::string> spaceGroup;
	::boost::optional<size_t> timesFound;

};

}}

#endif /* ADDITIONAL_DATA_H */
