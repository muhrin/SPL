/*
 * SymmetryFunctions.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SYMMETRY_FUNCTIONS_H
#define SYMMETRY_FUNCTIONS_H

// INCLUDES /////////////////////////////////
#include "spl/SSLib.h"

#include <vector>

#include <armadillo>

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {
class SymmetryGroup;
namespace symmetry {



::std::vector<unsigned int>
generateMultiplicities(const unsigned int numAtoms, const unsigned int numSymOps);

::std::vector<unsigned int>
generateMultiplicities(const unsigned int numAtoms, const ::std::vector<unsigned int>  & possibleMultiplicities);

void makeZRotation(::arma::mat44 & matOut, const double angle);
void makeReflection(::arma::mat44 & matOut, const ::arma::vec3 & normal);

void addZRotations(SymmetryGroup & groupOut, const unsigned int n);
void addReflection(SymmetryGroup & groupOut, const ::arma::vec3 & normal);

}
}
}

#endif /* SYMMETRY_FUNCTIONS_H */
