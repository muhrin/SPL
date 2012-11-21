/*
 * IoFunctions.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: muhrin
 */

// INCLUDES //////////////////////////////////
#include "io/IoFunctions.h"

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
  const ::std::string & wildString,
  ::std::vector< ::boost::filesystem::path> & outPaths,
  const fs::path & searchFolder)
{
  // Check that the search folder makes sense
  if(!fs::exists(searchFolder) || ! fs::is_directory(searchFolder))
    return false;

  const boost::regex resFileFilter(wildString);

  const fs::directory_iterator dirEnd; // Default ctor yields past-the-end
  for(fs::directory_iterator it(searchFolder); it != dirEnd; ++it )
  {
      // Skip if not a file
      if( !fs::is_regular_file(it->status())) continue;

      boost::smatch what;

      // Skip if no match
      if(!boost::regex_match(utility::fs::leafString(*it), what, resFileFilter)) continue;

      // File matches, store it
      outPaths.push_back(it->path());
  }

  return false;
}

}
}
