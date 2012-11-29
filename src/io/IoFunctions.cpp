/*
 * IoFunctions.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: muhrin
 */

// INCLUDES //////////////////////////////////
#include "io/IoFunctions.h"

#include <iomanip>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

// Local includes
#include <utility/BoostFilesystem.h>

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace io {

namespace fs = ::boost::filesystem;

bool getWildcardPaths(::std::string wildString, ::std::vector<fs::path> & outPaths)
{
  fs::path searchFolder(fs::current_path());
  const size_t firstAsterisk = wildString.find('*');

  if(firstAsterisk != ::std::string::npos)
  {
    size_t slashPos = wildString.rfind('/', firstAsterisk);
    if(slashPos == ::std::string::npos)
    {
      // Try the backslash
      slashPos = wildString.rfind('\\', firstAsterisk);
    }

    // Check if there's a path before the asteriks
    if(slashPos != ::std::string::npos)
    {
      searchFolder = wildString.substr(0, slashPos);
      wildString = wildString.substr(slashPos + 1);
    }
  }

  return getWildcardPaths(wildString, outPaths, searchFolder);

}

bool getWildcardPaths(
  ::std::string wildString,
  ::std::vector< ::boost::filesystem::path> & outPaths,
  const fs::path & searchFolder)
{
  // Check that the search folder makes sense
  if(!fs::exists(searchFolder) || ! fs::is_directory(searchFolder))
    return false;

  buildWildcardRegex(wildString);
  const boost::regex fileFilter(wildString);

  const fs::directory_iterator dirEnd; // Default ctor yields past-the-end
  for(fs::directory_iterator it(searchFolder); it != dirEnd; ++it )
  {
      // Skip if not a file
      if(!fs::is_regular_file(it->status()))
        continue;

      boost::smatch what;

      // Skip if no match
      if(!boost::regex_match(utility::fs::leafString(*it), what, fileFilter))
        continue;

      // File matches, store it
      outPaths.push_back(it->path());
  }

  return true;
}

void escapeCharacters(::std::string & regex)
{
  boost::replace_all(regex, "\\", "\\\\");
  boost::replace_all(regex, "^", "\\^");
  boost::replace_all(regex, ".", "\\.");
  boost::replace_all(regex, "$", "\\$");
  boost::replace_all(regex, "|", "\\|");
  boost::replace_all(regex, "(", "\\(");
  boost::replace_all(regex, ")", "\\)");
  boost::replace_all(regex, "[", "\\[");
  boost::replace_all(regex, "]", "\\]");
  boost::replace_all(regex, "*", "\\*");
  boost::replace_all(regex, "+", "\\+");
  boost::replace_all(regex, "?", "\\?");
  boost::replace_all(regex, "/", "\\/");
}

bool buildWildcardRegex(::std::string & pattern)
{
  // Escape all regex special chars
  escapeCharacters(pattern);

  // Convert chars '*?' back to their regex equivalents
  boost::replace_all(pattern, "\\?", ".");
  boost::replace_all(pattern, "\\*", ".*");

  return true;
}

int getPrecision(const double num, const unsigned int digitsAfterDecimal)
{
  int digits = digitsAfterDecimal + 1;
  if(num != 0.0)
    digits += (int)ceil(log10(abs(num)));
  return digits;
}

void writeToStream(::std::ostream & out, const double num, const unsigned digitsAfterDecimal)
{
  out << ::std::setprecision(getPrecision(num, digitsAfterDecimal)) << num;
}

}
}
