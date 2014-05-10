/*
 * GeneratorShape.cpp
 *
 *  Created on: Nov 25, 2013
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/GeneratorShape.h"

#include "spl/SSLibAssert.h"

namespace spl {
namespace build_cell {

const double GeneratorShapeWithShell::NO_SHELL = -1;

GeneratorShapeWithShell::GeneratorShapeWithShell():
    myShellThickness(NO_SHELL)
{
}

GeneratorShapeWithShell::GeneratorShapeWithShell(const double thickness):
    myShellThickness(thickness)
{
  SSLIB_ASSERT(thickness >= 0.0);
}

::arma::vec3
GeneratorShapeWithShell::randomPoint(const ::arma::mat44 * const transform) const
{
  if(!hasShell())
    return doRandomPoint(transform);
  else
  {
    // Use rejection sampling to get in that's in the shell
    ::arma::vec3 pt;
    do
    {
      pt = doRandomPoint(transform);
    } while(!isInShell(pt));
    return pt;
  }
}

bool
GeneratorShapeWithShell::hasShell() const
{
  return myShellThickness != NO_SHELL;
}

double
GeneratorShapeWithShell::shellThickness() const
{
  SSLIB_ASSERT(hasShell());
  return myShellThickness;
}

}
}
