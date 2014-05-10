/*
 * PointGroups.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef POINT_GROUPS_H
#define POINT_GROUPS_H

// INCLUDES ////////////

#include <vector>

#include <boost/optional.hpp>

#include "spl/build_cell/SymmetryGroup.h"

// DEFINITION ///////////////////////

namespace spl {
namespace build_cell {

struct PointGroupFamily
{
  enum Value
  {
    NONE,
    // Axial groups (need 'n' to specify how many rotational symmetries)
    C, S, Ci, Cs, Ch, Cv, D, Dd, Dh,
    // Polyhedral groups
    T, Td, Th, O, Oh, I, Ih
  };
};

typedef ::std::pair<PointGroupFamily::Value, unsigned int> PointGroup;

void generatePointGroup(
  SymmetryGroup & groupOut,
  const PointGroupFamily::Value family,
  const unsigned int n = 0);

bool getPointGroup(PointGroup & groupOut, const ::std::string & groupString);

::boost::optional<PointGroup>
getRandomPointGroup(const unsigned int numOps);

::std::vector<PointGroup>
getPossiblePointGroups(const unsigned int numOps);

}
}


#endif /* POINT_GROUPS_H */
