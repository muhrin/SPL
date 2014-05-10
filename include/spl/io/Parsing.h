/*
 * Parsing.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef PARSING_H
#define PARSING_H

// INCLUDES /////////////////////////////////////////////
#include <istream>

// TODO: Move following to _detail file
#include <string>
#include <boost/algorithm/string/find.hpp>

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////

namespace io {

extern const ::std::string PATTERN_FLOAT;
extern const ::std::string PATTERN_RANGE;
extern const ::std::string PATTERN_RANGE_CAPTURE;

bool findNextLine(
  ::std::string & matchingLine,
  ::std::istream & inputStream,
  const ::std::string & token,
  const bool caseSensitive = true
);

bool findLastLine(
  ::std::string & matchingLine,
  ::std::istream & inputStream,
  const ::std::string & token,
  const bool caseSensitive = true
);

template <typename FinderT> 
bool findLastLineFinder(
  ::std::string & matchingLine,
  ::std::istream & inputStream,
  const FinderT & finder
);

bool findFirstFloat(
  double & number,
  const ::std::string & line
);

::std::string getLastLine(::std::istream & is);
::std::string getLastNonEmptyLine(::std::istream & is);


// IMPLEMENTAITON ///////////////////////////////////////

template <typename FinderT> 
bool findLastLineFinder(
  ::std::string & matchingLine,
  ::std::istream & inputStream,
  const FinderT & finder
)
{
  int foundPos = -1;
  ::std::string line;
  while(::std::getline(inputStream, line))
  {
    if(::boost::find(line, finder))
    {
      foundPos = inputStream.tellg();
      matchingLine = line;
    }
  }
  if(foundPos != -1)
  {
    inputStream.clear(); // Clear the EoF flag
    inputStream.seekg(foundPos, inputStream.beg);
    return true;
  }
  return false;
}

}
}

#endif /* PARSING_H */
