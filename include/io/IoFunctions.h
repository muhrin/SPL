/*
 * IoFunctions.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef IO_FUNCTIONS_H
#define IO_FUNCTIONS_H

// INCLUDES /////////////////////////////////////////////

#include <string>
#include <vector>

#include <boost/filesystem.hpp>


// FORWARD DECLARES ////////////////////////////////

// DEFINES ////////////////////////////////////////


// FUNCTIONS ////////////////////////////////////////

namespace sstbx {
namespace io {

bool getWildcardPaths(
  ::std::string wildString,
  ::std::vector< ::boost::filesystem::path> & outPaths);

bool getWildcardPaths(
  const ::std::string & wildString,
  ::std::vector< ::boost::filesystem::path> & outPaths,
  const ::boost::filesystem::path & searchFolder);

}
}

#endif /* IO_FUNCTIONS_H */
