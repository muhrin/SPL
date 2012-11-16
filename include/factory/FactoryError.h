/*
 * FactoryError.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef FACTORY_ERROR_H
#define FACTORY_ERROR_H

// INCLUDES /////////////////////////////////////////////

#include <boost/exception/all.hpp>


// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace factory {

struct FactoryError : virtual ::boost::exception, virtual ::std::exception {};

}
}

#endif /* FACTORY_ERROR_H */
