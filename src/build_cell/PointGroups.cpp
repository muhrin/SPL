/*
 * PointGroups.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/PointGroups.h"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "spl/SSLibAssert.h"
#include "spl/build_cell/SymmetryFunctions.h"
#include "spl/build_cell/SymmetryGroup.h"
#include "spl/build_cell/SymmetryGroupBuilder.h"
#include "spl/common/Constants.h"
#include "spl/math/Random.h"

namespace spl {
namespace build_cell {

void
generatePointGroup(SymmetryGroup & groupOut,
    const PointGroupFamily::Value family, const unsigned int n)
{
  groupOut.reset();

  // Z is rotation axis
  ::arma::vec3 X, Y, Z;
  X << 1.0 << 0.0 << 0.0;
  Y << 0.0 << 1.0 << 0.0;
  Z << 0.0 << 0.0 << 1.0;

  ::arma::mat44 op;

  // Start with identity
  op.eye();
  groupOut.addOp(op);
  switch(family)
  {
  // Axial groups
  case PointGroupFamily::C:
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");

    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    break;
  case PointGroupFamily::Cv: // Vertical mirror plane
  {
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");
    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    symmetry::addReflection(groupOut, Y);
    const size_t numOps = groupOut.numOps();

    // Now combine rotations and reflections
    const ::arma::mat44 reflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * reflection);

    break;
  }

  case PointGroupFamily::Ch: // Horizontal mirror plane
  {
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");

    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    symmetry::addReflection(groupOut, Z);
    const size_t numOps = groupOut.numOps();

    // Now combine rotations and reflections
    const ::arma::mat44 reflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * reflection);

    //SymmetryGroupBuilder builder;
    //SymmetryGroupBuilder::Generator generator;

    //if(n > 1)
    //{
    //  symmetry::makeZRotation(generator, common::constants::TWO_PI / static_cast<double>(n));
    //  builder.addGenerator(generator);
    //}

    //symmetry::makeReflection(generator, Z);
    //builder.addGenerator(generator);

    //builder.build(groupOut);

    break;
  }

  case PointGroupFamily::S:
  {
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");

    symmetry::makeZRotation(op,
        common::constants::TWO_PI / static_cast< double>(n));
    op(2, 2) = -1.0; // Make it a rotation-reflection

    groupOut.addOp(op);
    for(size_t i = 1; i < n; ++i)
      groupOut.addOp(groupOut.getOp(i - 1) * op);

    break;
    case PointGroupFamily::Ci:
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");
    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    symmetry::addReflection(groupOut, X + Y + Z);
    const size_t numOps = groupOut.numOps();

    // Now combine rotations and reflections
    const ::arma::mat44 reflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * reflection);
  }
  break;

  case PointGroupFamily::Cs:
    symmetry::addReflection(groupOut, Z);
    break;

  case PointGroupFamily::D: // 2-fold (X/Y) mirror planes
  {
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");
    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    symmetry::addReflection(groupOut, X + Y);
    const size_t numOps = groupOut.numOps();

    // Now combine rotations and reflections
    const ::arma::mat44 reflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * reflection);
  }
  break;

  case PointGroupFamily::Dh: // 2-fold (X/Y) mirror planes and horizontal mirror plane
  {
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");
    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    symmetry::addReflection(groupOut, X + Y);

    // Now combine rotations and reflections
    size_t numOps = groupOut.numOps();
    const ::arma::mat44 reflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * reflection);

    symmetry::addReflection(groupOut, Z);
    // Now combine everything with this last reflection
    numOps = groupOut.numOps();
    const ::arma::mat44 hReflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * hReflection);
  }
  break;

  case PointGroupFamily::Dd:
  {
    SSLIB_ASSERT_MSG(n > 0, "Axial group must have rotations");
    if(n > 1)
      symmetry::addZRotations(groupOut, n);
    symmetry::addReflection(groupOut, X + Z);

    // Now combine rotations and reflections
    size_t numOps = groupOut.numOps();
    const ::arma::mat44 reflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * reflection);

    ::arma::mat44 rotZ, sigmaV;
    symmetry::makeZRotation(rotZ,
        common::constants::PI / static_cast< double>(n));
    symmetry::makeReflection(sigmaV, X);
    op = sigmaV * rotZ;
    groupOut.addOp(op);
    // Now combine everything with this last reflection
    numOps = groupOut.numOps();
    const ::arma::mat44 vReflection(groupOut.getOp(numOps - 1));
    for(size_t i = 1; i < numOps - 1; ++i)
      groupOut.addOp(groupOut.getOp(i) * vReflection);
  }
  break;

    // Polyhedral groups
  case PointGroupFamily::T:
    break;

  default:
    break;
  };
  // Finally generate the eigenvectors and multiplicities
  groupOut.generateEigenvectors();
}

bool
getPointGroup(PointGroup & groupOut, const ::std::string & groupString)
{
  static const boost::regex pgExpression("([CSDTOI])([[:digit:]]*)([hvids]*)");

  if(groupString.empty())
    return false;

  ::boost::smatch match;
  if(!::boost::regex_match(groupString, match, pgExpression)) // Check if we matched regex
    return false;

  groupOut.first = PointGroupFamily::NONE;
  groupOut.second = 0;

  // As with Perl regex match[i] gives: 0 = $&, i > 0 = $i
  const char familyChar(*match[1].first); // The first is guaranteed to be a single char
  ::std::string nFoldStr, mirror;
  nFoldStr.assign(match[2].first, match[2].second);
  mirror.assign(match[3].first, match[3].second);

  // Is there an n-fold rotation axis?
  unsigned int nFold = 0;
  if(!nFoldStr.empty())
  {
    try
    {
      nFold = ::boost::lexical_cast< unsigned int>(nFoldStr);
    }
    catch(const ::boost::bad_lexical_cast & /*e*/)
    {
      return false;
    }
  }

  switch(familyChar)
  {
  case 'C':
    if(mirror.empty())
      groupOut.first = PointGroupFamily::C;
    else if(mirror == "i")
      groupOut.first = PointGroupFamily::Ci;
    else if(mirror == "s")
      groupOut.first = PointGroupFamily::Cs;
    else if(mirror == "h")
      groupOut.first = PointGroupFamily::Ch;
    else if(mirror == "v")
      groupOut.first = PointGroupFamily::Cv;
    else
      return false;

    groupOut.second = nFold;

    break;

  case 'S':
    groupOut.first = PointGroupFamily::S;
    groupOut.second = nFold;
    break;

  case 'D':
    if(mirror.empty())
      groupOut.first = PointGroupFamily::D;
    else if(mirror == "d")
      groupOut.first = PointGroupFamily::Dd;
    else if(mirror == "h")
      groupOut.first = PointGroupFamily::Dh;
    else
      return false;
    groupOut.second = nFold;

    break;

  default:
    return false;

  };

  return true;
}

::boost::optional< ::std::pair< PointGroupFamily::Value, unsigned int> >
getRandomPointGroup(const unsigned int numOps)
{
  ::boost::optional< ::std::pair< PointGroupFamily::Value, unsigned int> > group;

  const ::std::vector< ::std::pair< PointGroupFamily::Value, unsigned int> > possibleGroups(
      getPossiblePointGroups(numOps));

  if(!possibleGroups.empty())
    group.reset(
        possibleGroups[math::randu< size_t>(0, possibleGroups.size() - 1)]);

  return group;
}

::std::vector< ::std::pair< PointGroupFamily::Value, unsigned int> >
getPossiblePointGroups(const unsigned int numOps)
{
  ::std::vector< ::std::pair< PointGroupFamily::Value, unsigned int> > possibleGroups;
  if(numOps == 0)
    return possibleGroups;

  // Every numOps can have a Cn group
  possibleGroups.push_back(::std::make_pair(PointGroupFamily::C, numOps));
  if(numOps % 2 == 0) // Even
  {
    possibleGroups.push_back(
        ::std::make_pair(PointGroupFamily::Ch, numOps / 2));
    possibleGroups.push_back(
        ::std::make_pair(PointGroupFamily::Cv, numOps / 2));
    possibleGroups.push_back(::std::make_pair(PointGroupFamily::D, numOps / 2));
    if(numOps >= 4)
    {
      possibleGroups.push_back(
          ::std::make_pair(PointGroupFamily::Dh, numOps / 4));
      possibleGroups.push_back(
          ::std::make_pair(PointGroupFamily::Dd, numOps / 4));
    }
  }
  else // Odd
  {

  }

  return possibleGroups;
}

}
}
