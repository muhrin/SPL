/*
 * ResourceLocator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef RESOURCE_LOCATOR_H
#define RESOURCE_LOCATOR_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"

#include <string>

#include <boost/filesystem.hpp>


// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx {
namespace io {

class ResourceLocator
{
public:

  ResourceLocator();
  /* implicit */ ResourceLocator(const ::boost::filesystem::path & path);
  ResourceLocator(const ::boost::filesystem::path & path, const ::std::string & resourceId);

  bool set(const ::std::string & locatorString);

  ::std::string string() const;

  const ::boost::filesystem::path & path() const;
  void setPath(const ::boost::filesystem::path & path);

  const ::std::string & id() const;
  void setId(const ::std::string & resourceId);

  bool empty() const;

  ResourceLocator & operator =(const ResourceLocator & rhs);

private:

  static const ::std::string ID_DELIMITER;

  ::boost::filesystem::path myPath;
  ::std::string myResourceId;
};

}
}

#endif /* RESOURCE_LOCATOR_H */
