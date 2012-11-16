/*
 * YamlCommon.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef YAML_COMMON_H
#define YAML_COMMON_H

// INCLUDES //////////////////////////////////
#include "SSLib.h"

#ifdef SSLIB_USE_YAML

#include <string>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "potential/SimplePairPotential.h"

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace factory {

/** The id type used by elements in a yaml document */
typedef ::std::string     KwTyp;

struct ArmaTriangularMat
{
  ::arma::mat mat;
};

}
}

// Converters


// Some custom YAML converters
namespace YAML {

  // Simple pair potential combining rules
  template<>
  struct convert< ::sstbx::potential::SimplePairPotential::CombiningRule>
  {
    typedef ::sstbx::potential::SimplePairPotential Pp;
    static Node encode(const Pp::CombiningRule & rhs)
    {
      Node node;

      switch(rhs)
      {
      case Pp::NONE:
        {
          node = "none";
          break;
        }
      case Pp::LORENTZ:
        {
          node = "lorentz";
          break;
        }
      case Pp::BERTHELOT:
        {
          node = "berthelot";
          break;
        }
      case Pp::LORENTZ_BERTHELOT:
        {
          node = "lorentzBerthelot";
          break;
        }
      case Pp::CUSTOM:
        {
          node = "custom";
          break;
        }
      default:
        {
          node = "none";
          break;
        }
      }

      return node;
     }

    static bool decode(const Node& node, Pp::CombiningRule & rhs)
    {
      if(!node.IsScalar())
        return false;

      const ::std::string & value = node.Scalar();

      if(value == "none")
      {
        rhs = Pp::NONE;
      }
      else if(value == "lorentz")
      {
        rhs = Pp::LORENTZ;
      }
      else if(value == "berthelot")
      {
        rhs = Pp::BERTHELOT;
      }
      else if(value == "lorentzBerthelot")
      {
        rhs = Pp::LORENTZ_BERTHELOT;
      }
      else if(value == "custom")
      {
        rhs = Pp::CUSTOM;
      }
      else
        return false;

      return true;
    }
  };

  // Armadillo triangular matrices
  template<>
  struct convert< ::sstbx::factory::ArmaTriangularMat>
  {
    static Node encode(const ::sstbx::factory::ArmaTriangularMat & rhs)
    {
      Node node;
      for(size_t i = 0; i < rhs.mat.n_rows; ++i)
      {
        for(size_t j = i; j < rhs.mat.n_cols; ++j)
        {
          node.push_back(rhs.mat(i, j));
        }
      }
      return node;
     }

    static bool decode(const Node & node, ::sstbx::factory::ArmaTriangularMat & rhs)
    {
      if(!node.IsSequence())
        return false;

      // Check if this is good for a triangular matrix
      const unsigned int nElements = node.size();
      const double dSize = 0.5 * (-1.0 + ::std::sqrt(1.0 + 8.0 * node.size()));
      const unsigned int iSize = (int)(dSize + 0.5);
      const unsigned int iTot = iSize * (iSize + 1) / 2;

      // Check that the double/int conversion hasn't caused problems
      if(iTot != nElements)
        return false;

      rhs.mat.set_size(iSize, iSize);
      size_t k = 0;
      for(size_t i = 0; i < iSize; ++i)
      {
        for(size_t j = i; j < iSize; ++j)
        {
          rhs.mat(i, j) = node[k].as<double>();
          ++k;
        }
      }
      rhs.mat = ::arma::symmatu(rhs.mat);

      return true;
    }
  };

  // Armadillo vectors
  template<>
  struct convert<arma::vec>
  {
    static Node encode(const arma::vec & rhs)
    {
      Node node;
      for(size_t i = 0; i < rhs.n_rows; ++i)
      {
        node.push_back(rhs(i));
      }
      return node;
     }

    static bool decode(const Node& node, arma::vec & rhs)
    {
      if(!(node.IsSequence() || node.IsScalar()))
        return false;

      if(node.IsSequence())
      {
        rhs.set_size(node.size());
        for(size_t i = 0; i < node.size(); ++i)
        {
          rhs(i) = node[i].as<double>();
        }
      }
      else if(node.IsScalar())
      {
        // Maybe it is a string separated by spaces
        typedef boost::tokenizer<boost::char_separator<char> > Tok;
        const boost::char_separator<char> sep(" ");

        Tok tok(node.as< ::std::string>(), sep);
        ::std::vector<double> values;
        BOOST_FOREACH(const ::std::string & value, tok)
        {
          try
          {
            values.push_back(::boost::lexical_cast<double>(value));
          }
          catch(const ::boost::bad_lexical_cast & /*exception*/)
          {
            return false;
          }
        }
      }
      else
        return false;

      return true;
    }
  };
}


#endif /* YAML_COMMON_H */

#endif /* SSLIB_USE_YAML */
