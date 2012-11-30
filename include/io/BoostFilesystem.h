/*
 * BoostFilesystem.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef BOOST_FILESYSTEM_H
#define BOOST_FILESYSTEM_H

// INCLUDES /////////////////////////////////////////////

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/version.hpp>

// DEFINES ///////////////////////////////////////////////

/** Should we use version 2 of the boost filesystem library? */
#if (BOOST_VERSION / 100000) <= 1 && ((BOOST_VERSION / 100) % 1000) <= 45
#  define SSLIB_USE_BOOSTFS_V2
#endif

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace io {

// Return path when appended to a_From will resolve to same as a_To
::boost::filesystem::path make_relative(
  ::boost::filesystem::path a_From,
  ::boost::filesystem::path a_To );

::std::string stemString(const ::boost::filesystem::path & path);

::std::string leafString(const ::boost::filesystem::path & path);

::boost::filesystem::path append(
  ::boost::filesystem::path path,
  const ::boost::filesystem::path::iterator & begin,
  const ::boost::filesystem::path::iterator & end
);

::boost::filesystem::path & appendTo(
  ::boost::filesystem::path & path,
  ::boost::filesystem::path::iterator begin,
  const ::boost::filesystem::path::iterator & end
);

::boost::filesystem::path
absolute(const ::boost::filesystem::path & p);
bool isAbsolute(const ::boost::filesystem::path & toCheck);


}
}

#endif /* BOOST_FILESYSTEM_H */
