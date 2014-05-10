/*
 * BoostFilesystem.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/io/BoostFilesystem.h"

namespace spl {
namespace io {

namespace boostfs = boost::filesystem;

ScopedFile::ScopedFile(const std::string & path):
    myPath(path)
{
  createFile(myPath);
}

ScopedFile::ScopedFile(const boostfs::path & path):
    myPath(path)
{
  createFile(myPath);
}

const boostfs::path &
ScopedFile::get() const
{
  return myPath;
}

ScopedFile::~ScopedFile()
{
  boostfs::remove_all(myPath);
}

// Return path when appended to a_From will resolve to same as a_To
boostfs::path make_relative(
  boostfs::path a_From,
  boostfs::path a_To )
{
  a_From = io::absolute(a_From);
  a_To = io::absolute(a_To);

  boostfs::path ret;
  boostfs::path::const_iterator itrFrom( a_From.begin() ), itrTo( a_To.begin() );
  
  // Find common base
  for(boostfs::path::const_iterator toEnd( a_To.end() ), fromEnd( a_From.end() ) ;
    itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo;
    ++itrFrom, ++itrTo );

  // Navigate backwards in directory to reach previously found base
  for( boostfs::path::const_iterator fromEnd( a_From.end() ); itrFrom != fromEnd; ++itrFrom )
  {
    if( (*itrFrom) != "." )
      ret /= "..";
  }

  // Now navigate down the directory branch
  return appendTo(ret, itrTo, a_To.end());
}

::std::string stemString(const boostfs::path & path)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return path.stem();
#else
  return path.stem().string();
#endif
}

::std::string leafString(const boost::filesystem::path & path)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return path.filename();
#else
  return path.filename().string();
#endif
}

boostfs::path append(
  boostfs::path path,
  const boostfs::path::iterator & begin,
  const boostfs::path::iterator & end
)
{
  return appendTo(path, begin, end);
}

boostfs::path & appendTo(
  boostfs::path & path,
  boostfs::path::iterator begin,
  const boostfs::path::iterator & end
)
{
  for( ; begin != end ; ++begin )
    path /= *begin;
  return path;
}

boostfs::path absolute(const boostfs::path & p)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return boostfs::system_complete(p);
#else
  return boostfs::absolute(p);
#endif
}

bool
isAbsolute(const boostfs::path & toCheck)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return toCheck.is_complete();
#else
  return toCheck.is_absolute();
#endif
}

bool
createFile(const boost::filesystem::path & toCreate)
{
  boostfs::ofstream file(toCreate);
  file.close();
  return boostfs::exists(toCreate);
}

bool
createAndChangeCurrentPath(const boost::filesystem::path & path)
{
  boostfs::create_directories(path);
#ifdef SSLIB_USE_BOOSTFS_V2
  boostfs::current_path(path);
  return true;
#else
  boost::system::error_code error;
  boostfs::current_path(path, error);
  return !error;
#endif
}

}
}
