/*
 * generao.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "schemer_test.h"

#include <schemer/Schemer.h>

enum Gender
{
  MALE,
  FEMALE
};

SCHEMER_ENUM(GenderSchema, Gender)
{
  enumeration("male", MALE);
  enumeration("female", FEMALE);
}

BOOST_AUTO_TEST_CASE(Enumeration)
{
  // SETTINGS //////

  const Gender ORIGINAL_GENDER = MALE;
  YAML::Node genderNode;
  schemer::serialise(ORIGINAL_GENDER, &genderNode);

  BOOST_REQUIRE(genderNode.Scalar() == "male");

  Gender g;
  schemer::ParseLog log;
  schemer::parse(genderNode, &g, &log);

  BOOST_REQUIRE(g == ORIGINAL_GENDER);
}
