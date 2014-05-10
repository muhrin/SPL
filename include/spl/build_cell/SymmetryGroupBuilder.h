/*
 * SymmetryGroupBuilder.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SYMMETRY_GROUP_BUILDER_H
#define SYMMETRY_GROUP_BUILDER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <vector>

#include <boost/noncopyable.hpp>

#include <armadillo>

// DEFINITION ///////////////////////

namespace spl {
namespace build_cell {
class SymmetryGroup;

class SymmetryGroupBuilder : ::boost::noncopyable
{
public:
  typedef ::arma::mat44 Generator;
  typedef ::std::vector<Generator> Generators;
  typedef Generators::iterator GeneratorIterator;

  SymmetryGroupBuilder();

  GeneratorIterator beginGenerators();
  GeneratorIterator endGenerators();
  bool addGenerator(const Generator & generator);
  bool removeGenerator(const GeneratorIterator it);

  void build(SymmetryGroup & groupOut) const;

private:
  static const Generator IDENTITY;

  Generators myGenerators;
};

}
}


#endif /* SYMMETRY_GROUP_H */
