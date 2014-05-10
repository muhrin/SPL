/*
 * TranscodeIo.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef TRANSCODE_IO_H
#define TRANSCODE_IO_H

// INCLUDES //////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_YAML

#include <boost/filesystem/path.hpp>

#include <yaml-cpp/yaml.h>


// NAMESPACES ////////////////////////////////

// Some custom YAML transcoders
namespace YAML {

template <>
struct convert< ::boost::filesystem::path>
{
  static Node encode(const ::boost::filesystem::path & path);
  static bool decode(const Node & node, ::boost::filesystem::path & path);
};

}

#endif /* SPL_WITH_YAML */
#endif /* TRANSCODE_IO_H */
