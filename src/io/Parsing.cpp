/*
 * Parsing.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/Parsing.h"

#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

// DEFINES /////////////////////////////////


// NAMESPACES ////////////////////////////////


namespace spl {
namespace io {

const ::std::string PATTERN_FLOAT("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
const ::std::string PATTERN_RANGE(PATTERN_FLOAT + "[[:blank:]]*(~[[:blank:]]*" + PATTERN_FLOAT + ")?");
const ::std::string PATTERN_RANGE_CAPTURE("(" + PATTERN_FLOAT + ")[[:blank:]]*(~[[:blank:]]*(" + PATTERN_FLOAT + "))?");

bool findNextLine(
  ::std::string & matchingLine,
  ::std::istream & inputStream,
  const ::std::string & token,
  const bool caseSensitive
)
{
  ::std::string line;
  bool found;
  while(::std::getline(inputStream, line))
  {
    found = caseSensitive ? ::boost::find_first(line, token) : ::boost::ifind_first(line, token);
    if(found)
    {
      matchingLine = line;
      return true;
    }
  }
  return false;
}

bool findLastLine(
  ::std::string & matchingLine,
  ::std::istream & inputStream,
  const ::std::string & token,
  const bool caseSensitive
)
{
  int foundPos = -1;
  ::std::string line;
  bool found;
  while(::std::getline(inputStream, line))
  {
    found = caseSensitive ? ::boost::find_first(line, token) : ::boost::ifind_first(line, token);
    if(found)
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

bool findFirstFloat(
  double & number,
  const ::std::string & line
)
{
  static const ::boost::regex RE_FLOAT("([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)");

  ::boost::smatch match;
  if(::boost::regex_search(line, match, RE_FLOAT))
  {
    const ::std::string numString(match[1].first, match[1].second);;
    try
    {
      number = ::boost::lexical_cast<double>(numString);
      return true;
    }
    catch(const ::boost::bad_lexical_cast & /*e*/)
    {
      return false;
    }
  }
  return false;
}

::std::string getLastLine(::std::istream & is)
{
  // Go to just before EoF
  is.seekg(-1, is.end);

  bool found = false;
  char ch;
  while(!found)
  {
    // Get the current character
    is.get(ch);

    // Are we at or before the first character
    if((int)is.tellg() <= 1)
    {
      is.seekg(0); // First line is last line
      found = true;
    }
    else if(ch == '\n')
      found = true;
    else
      is.seekg(-2, is.cur); // Move two backwards so we read the next char
  }
  ::std::string lastLine;
  ::std::getline(is, lastLine);
  return lastLine;
}

::std::string getLastNonEmptyLine(::std::istream & is)
{
  // Go to just before EoF
  is.seekg(-1, is.end);

  bool notEmpty = false;
  bool found = false;
  char ch;
  while(!found)
  {
    // Get the current character
    is.get(ch);
    notEmpty |= (::std::isspace(ch) == 0);

    // Are we at or before the first character
    if((int)is.tellg() <= 1)
    {
      is.seekg(0); // First line is last line
      found = true;
    }
    else if(notEmpty && ch == '\n')
    {
      found = true;
    }
    else
      is.seekg(-2, is.cur); // Move two backwards so we read the next char
  }
  ::std::string lastLine;
  ::std::getline(is, lastLine);
  return lastLine;
}

}
}
