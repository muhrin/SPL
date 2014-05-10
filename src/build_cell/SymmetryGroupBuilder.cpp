/*
 * SymmetryGroupBuilder.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/SymmetryGroupBuilder.h"


#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"
#include "spl/build_cell/SymmetryGroup.h"
#include "spl/utility/StableComparison.h"

namespace spl {
namespace build_cell {

namespace comp = utility::stable;

const SymmetryGroupBuilder::Generator
SymmetryGroupBuilder::IDENTITY(::arma::eye< ::arma::mat>(4,4));

SymmetryGroupBuilder::SymmetryGroupBuilder()
{
  myGenerators.push_back(IDENTITY);
}

SymmetryGroupBuilder::GeneratorIterator
SymmetryGroupBuilder::beginGenerators()
{
  return myGenerators.begin();
}

SymmetryGroupBuilder::GeneratorIterator
SymmetryGroupBuilder::endGenerators()
{
  return myGenerators.end();
}

bool SymmetryGroupBuilder::addGenerator(const Generator & generator)
{
  // TODO: CHECK IF THE GENERATOR ALREADY EXISTS
  myGenerators.push_back(generator);
  return true;
}

bool SymmetryGroupBuilder::removeGenerator(const GeneratorIterator it)
{
  // TODO: Check that it is not the ideneity (which can't be removed)
  myGenerators.erase(it);
  return true;
}

void SymmetryGroupBuilder::build(SymmetryGroup & groupOut) const
{
  groupOut.reset();

  // Put all the generators in the group, these will be kept in the
  // order that they were inserted in
  BOOST_FOREACH(const Generator & generator, myGenerators)
    groupOut.addOp(generator);

  for(size_t i = 1; i < groupOut.numOps(); ++i)
  {
    for(size_t j = 1; j < groupOut.numOps(); ++j)
    {
      const ::arma::mat44 result(groupOut.getOp(i) * groupOut.getOp(j));
      if(!groupOut.isInGroup(result))
        groupOut.addOp(result);
    }
  }
}

}
}
