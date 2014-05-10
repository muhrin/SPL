/*
 * TranscodeGeneral.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/yaml/TranscodeGeneral.h"

#include <iomanip>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "spl/utility/IndexingEnums.h"

// NAMESPACES ////////////////////////////////
namespace ssu = ::spl::utility;
namespace ssy = ::spl::yaml;

namespace YAML {

// Armadillo triangular matrices
Node convert<ssy::ArmaTriangularMat>::encode(
  const ssy::ArmaTriangularMat & rhs)
{
  Node node;
  ssy::VecAsString<double>::Type flatMatrix;

  for(size_t i = 0; i < rhs->n_rows; ++i)
  {
    for(size_t j = i; j < rhs->n_cols; ++j)
      flatMatrix->push_back((*rhs)(i, j));
  }

  node = flatMatrix;
  return node;
}

bool convert< ssy::ArmaTriangularMat>::decode(
  const Node & node, ssy::ArmaTriangularMat & rhs)
{
  typedef ssy::VecAsString<double>::Type DoublesVec;

  DoublesVec flatMatrix;
  try
  {
    flatMatrix = node.as<DoublesVec>();
  }
  catch(const YAML::TypedBadConversion<DoublesVec> & /*e*/)
  {
    return false;
  }

  if(flatMatrix->empty())
    return false;

  // Check if this is good for a triangular matrix
  const int nElements = flatMatrix->size();
  const double dSize = 0.5 * (-1.0 + ::std::sqrt(1.0 + 8.0 * nElements));
  const int iSize = static_cast<int>(dSize + 0.5);
  const int iTot = iSize * (iSize + 1) / 2;

  // Check that the double/int conversion hasn't caused problems
  if(iTot != nElements)
    return false;

  rhs->set_size(iSize, iSize);
  size_t k = 0;
  for(int i = 0; i < iSize; ++i)
  {
    for(int j = i; j < iSize; ++j)
    {
      (*rhs)(i, j) = (*flatMatrix)[k];
      ++k;
    }
  }
  (*rhs) = ::arma::symmatu(*rhs);

  return true;
}

}


