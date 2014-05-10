/*
 * TranscodeIo.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/yaml/TranscodeIo.h"

// NAMESPACES ////////////////////////////////

namespace YAML {

Node convert< ::boost::filesystem::path>::encode(const ::boost::filesystem::path & rhs)
{
  Node node;
  node = rhs.string();
  return node;
}

bool convert< ::boost::filesystem::path>::decode(const Node & node, ::boost::filesystem::path & rhs)
{
  if(!node.IsScalar())
    return false;
  rhs = node.Scalar();
  return true;
}

}


