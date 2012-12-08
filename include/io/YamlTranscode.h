/*
 * YamlTranscode.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef YAML_TRANSCODE_H
#define YAML_TRANSCODE_H

// INCLUDES //////////////////////////////////
#include "SSLib.h"

#ifdef SSLIB_USE_YAML

#include <sstream>
#include <string>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "common/UnitCell.h"
#include "factory/SsLibYamlKeywords.h"
#include "io/IoFunctions.h"
#include "potential/SimplePairPotential.h"
#include "utility/IndexingEnums.h"

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

  //template<>
  //struct convert< ::sstbx::common::Atom>
  //{
  //  static Node encode(const ::sstbx::common::Atom & atom)
  //  {
  //    namespace kw = ::sstbx::factory::sslib_yaml_keywords;
  //    using namespace utility::cart_coords_enum;

  //    YAML::Node atomNode;

  //    // Species
  //    const ::std::string * const species = mySpeciesDb.getSymbol(atom.getSpecies());
  //    if(species)
  //      atomNode[kw::STRUCTURE__ATOMS__SPEC] = *species;

  //    // Position
  //    atomNode[kw::STRUCTURE__ATOMS__POS] = atom.getPosition();

  //    return atomNode;

  //  }

  //}

  template<>
  struct convert< ::sstbx::common::UnitCell>
  {
    static Node encode(const ::sstbx::common::UnitCell & cell)
    {
      namespace kw = ::sstbx::factory::sslib_yaml_keywords;
      using namespace ::sstbx::utility::cell_params_enum;

      Node node;
      const double (&params)[6] = cell.getLatticeParams();
      ::std::stringstream ss;
      for(size_t i = A; i <= GAMMA; ++i)
      {
        if(i != A)
          ss << " ";
        ::sstbx::io::writeToStream(ss, params[i], 10);
      }
      node[kw::STRUCTURE__CELL__ABC] = ss.str();

      node[kw::STRUCTURE__CELL__VOL] = ::boost::lexical_cast<double>(cell.getVolume());

      return node;
     }

    static bool decode(const Node & node, ::sstbx::common::UnitCell & cell)
    {
      namespace kw = ::sstbx::factory::sslib_yaml_keywords;
      using namespace ::sstbx::utility::cell_params_enum;

      if(!node[kw::STRUCTURE__CELL__ABC])
        return false;

      double params[6];

      typedef boost::tokenizer<boost::char_separator<char> > Tok;
      const boost::char_separator<char> sep(" ");

      Tok tok(node[kw::STRUCTURE__CELL__ABC].as< ::std::string>(), sep);

      size_t param = 0;
      for(Tok::iterator it = tok.begin(), end = tok.end();
        it != end; ++param, ++it)
      {
        if(param > GAMMA)
          return false; // Expect six parameters

        try
        {
          params[param] = ::boost::lexical_cast<double>(*it);
        }
        catch(const ::boost::bad_lexical_cast & /*exception*/)
        {
          return false;
        }
      }

      if(param <= GAMMA)
        return false; // Expect six parameters

      cell.setLatticeParams(params);

      return true;
    }
  };

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
  struct convert<arma::vec3>
  {
    static Node encode(const arma::vec3 & rhs)
    {
      using namespace ::sstbx::utility::cart_coords_enum;

      Node node;
      ::std::stringstream ss;
      ss << ::std::setprecision(12) << rhs(X) << " " << rhs(Y) << " " << rhs(Z);
      node = ss.str();
      return node;
     }

    static bool decode(const Node & node, arma::vec3 & rhs)
    {
      using namespace ::sstbx::utility::cart_coords_enum;

      if(node.IsSequence())
      {
        rhs(X) = node[(int)X].as<double>();
        rhs(Y) = node[(int)Y].as<double>();
        rhs(Z) = node[(int)Z].as<double>();
      }
      else if(node.IsScalar())
      {
        // Maybe it is a string separated by spaces
        typedef boost::tokenizer<boost::char_separator<char> > Tok;
        const boost::char_separator<char> sep(" ");

        Tok tok(node.as< ::std::string>(), sep);

        size_t coord = 0;
        for(Tok::const_iterator it = tok.begin(), end = tok.end();
          it != end; ++coord, ++it)
        {
          if(coord > Z)
            return false; // Expecting three coordinates

          try
          {
            rhs(coord) = ::boost::lexical_cast<double>(*it);
          }
          catch(const ::boost::bad_lexical_cast & /*exception*/)
          {
            return false;
          }
        }

        if(coord <= Z)
          return false; // Expecting three coordinates
      }
      else
        return false;

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

#endif /* YAML_TRANSCODE_H */
