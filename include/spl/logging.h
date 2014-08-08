/*
 * logging.h
 *
 *  Created on: Aug 6, 2014
 *      Author: Martin Uhrin
 */

#ifndef SPL__LOGGING_H_
#define SPL__LOGGING_H_

// INCLUDES ///////////////////////////////////////////////
#include "spl/config.h"
#ifdef SPL_USE_BOOST_LOG

#include <boost/log/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace spl {

#ifdef SSLIB_ENABLE_THREAD_AWARE
  typedef boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level> Logger;
#else
  typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level> Logger;
#endif

// Create the global logger
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, Logger)

}



#endif // SPL_USE_BOOST_LOG
#endif /* SPL__LOGGING_H_ */
