/*
 * Types.h
 *
 *
 *  Created on: Jan 6, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_TYPES_H
#define SCHEMER_TYPES_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include <boost/optional.hpp>

#include "schemer/Scalar.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {

typedef Scalar< ::std::string> String;
typedef Scalar< int> Int;

}

#endif /* SCHEMER_TYPES_H */
