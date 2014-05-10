/*
 * TranscodeGeneral.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef TRANSCODE_GENERAL_DETAIL_H
#define TRANSCODE_GENERAL_DETAIL_H

// INCLUDES //////////////////////////////////
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "spl/io/Parsing.h"
#include "spl/utility/Armadillo.h"

// NAMESPACES ////////////////////////////////
namespace ssy = spl::yaml;

namespace YAML {

template< typename T>
  Node
  convert< spl::yaml::TypeWrapper< std::vector< T> > >::encode(
      const typename ssy::VecAsString< T>::Type & vector)
  {
    Node node;
    BOOST_FOREACH(const T & value, *vector)
      node.push_back(value);

    return node;
  }

template< typename T>
  bool
  convert< spl::yaml::TypeWrapper< std::vector< T> > >::decode(
      const Node & node, typename ssy::VecAsString< T>::Type & vector)
  {
    if(node.IsSequence())
    {
      BOOST_FOREACH(const Node & value, node)
      {
        try
        {
          vector->push_back(value.as< T>());
        }
        catch(const YAML::TypedBadConversion< T> & /*e*/)
        {
          return false;
        }
      }
    }
    else if(node.IsScalar())
    {
      typedef boost::tokenizer< boost::char_separator< char> > Tok;
      const boost::char_separator< char> sep(" ");

      // Treat each entry in the string as a scalar node and
      // try conversion on that

      // Have to 'save' the string otherwise tokenizer doesn't like it
      const std::string tokenString(node.Scalar());
      Tok tok(tokenString, sep);
      BOOST_FOREACH(const std::string & entry, tok)
      {
        Node entryNode;
        entryNode = entry;
        try
        {
          vector->push_back(entryNode.as< T>());
        }
        catch(const YAML::TypedBadConversion< T> & /*e*/)
        {
          return false;
        }
      }
    }
    else
      return false;

    return true;
  }

template< typename T>
  Node
  convert< arma::Mat< T> >::encode(const arma::Mat< T> & rhs)
  {
    Node node;

    // TODO: Figure out a way to print without \n's from raw_print
    std::stringstream ss;
    rhs.raw_print(ss);
    node = ss.str();

    return node;
  }

template< typename T>
  bool
  convert< arma::Mat< T> >::decode(const Node & node, arma::Mat< T> & rhs)
  {
    if(!node.IsScalar())
      return false;

    std::stringstream ss(node.Scalar());
    try
    {
      ss >> rhs;
    }
    catch(const std::logic_error & /*e*/)
    {
      return false;
    }

    return true;
  }

}

#endif /* TRANSCODE_GENERAL_DETAIL_H */
