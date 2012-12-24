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

#include <ostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>


// FORWARD DECLARES ////////////////////////////////

// DEFINES ////////////////////////////////////////


// FUNCTIONS ////////////////////////////////////////

namespace sstbx {
namespace io {
class StructureReadWriteManager;

bool getWildcardPaths(
  ::std::string wildString,
  ::std::vector< ::boost::filesystem::path> & outPaths);

bool getWildcardPaths(
  ::std::string wildString,
  ::std::vector< ::boost::filesystem::path> & outPaths,
  const ::boost::filesystem::path & searchFolder);

void escapeCharacters(::std::string & regex);

bool buildWildcardRegex(::std::string & pattern);

int getPrecision(const double num, const unsigned int digitsAfterDecimal);

void writeToStream(::std::ostream & out, const double num, const unsigned digitsAfterDecimal);

::std::string toString(const double num, const unsigned digitsAfterDecimal);

void populateReadWriteManager(StructureReadWriteManager & manager);

}
}

#endif /* IO_FUNCTIONS_H */
