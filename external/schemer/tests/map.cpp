/*
 * map.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "schemer_test.h"

#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>

#include <schemer/Schemer.h>

// USING ////////////////////////////
using std::string;

// CONSTANTS ////////////////////////
static const string KEY_NAME = "name";
static const string KEY_AGE = "age";
static const string KEY_NICKNAME = "nickname";
static const string KEY_PHONE_NO = "phone_no";
static const string KEY_EMPLOYEE_NO = "employeeNo";

static const string BOB_NAME = "Bob";
static const int BOB_AGE = 29;
static const int BOB_PHONE_NO = 123456789;

struct Person_t
{
  bool
  operator ==(const Person_t & rhs) const
  {
    return name == rhs.name && age == rhs.age && nickname == rhs.nickname
        && phone_no == rhs.phone_no;
  }
  int age;
  std::string name;
  boost::optional< std::string> nickname;
  boost::optional< int> phone_no;
};

struct Employee_t : public Person_t
{
  bool
  operator ==(const Employee_t & rhs) const
  {
    if(!Person_t::operator ==(rhs))
      return false;
    return employeeNo == rhs.employeeNo;
  }
  int employeeNo;
};

// SCHEMAS ///////////////////////////////////
SCHEMER_MAP(Person, Person_t)
{
  element(KEY_NAME, &Person_t::name);
  element(KEY_AGE, &Person_t::age);
  element(KEY_NICKNAME, &Person_t::nickname);
  element(KEY_PHONE_NO, &Person_t::phone_no);
}

SCHEMER_MAP(Employee, Employee_t)
{
  extends< Person>();
  element(KEY_EMPLOYEE_NO, &Employee_t::employeeNo)->defaultValue(-1);
}

struct Test_t
{
  int someValue;
};

struct Test : public schemer::HeteroMap< Test_t>
{
  Test()
  {
    element("someValue", &Test_t::someValue);
  }
};

SCHEMER_REGISTER(Test, Test_t);

const Person_t &
bob();

BOOST_AUTO_TEST_CASE(HeteroMapTest)
{
  using std::string;

  // SETTINGS //

  schemer::ParseLog parse;

  YAML::Node personNode;
  schemer::serialise(bob(), &personNode);

  // Check that the node got all the data from ORIGINAL_PERSON
  BOOST_REQUIRE(personNode.IsMap());
  BOOST_REQUIRE(personNode[KEY_NAME].Scalar() == bob().name);
  BOOST_REQUIRE(personNode[KEY_AGE].as< int>() == bob().age);
  BOOST_REQUIRE(!personNode[KEY_NICKNAME]);
  BOOST_REQUIRE(personNode[KEY_PHONE_NO].as< int>() == bob().phone_no);

  Person_t p;
  schemer::parse(personNode, &p, &parse);

  // Check that the node was successfully converted back to a person
  BOOST_REQUIRE(p == bob());

  YAML::Node bobEmployeeNode;
  Employee_t bobEmployee;
  bobEmployee.age = bob().age;
  bobEmployee.employeeNo = 1234;
  bobEmployee.name = bob().name;
  bobEmployee.nickname = bob().nickname;
  bobEmployee.phone_no = bob().phone_no;

  schemer::serialise(bobEmployee, &bobEmployeeNode);
  BOOST_REQUIRE(bobEmployeeNode.IsMap());
  BOOST_REQUIRE(bobEmployeeNode[KEY_NAME].Scalar() == bob().name);
  BOOST_REQUIRE(bobEmployeeNode[KEY_AGE].as< int>() == bob().age);
  BOOST_REQUIRE(!bobEmployeeNode[KEY_NICKNAME]);
  BOOST_REQUIRE(bobEmployeeNode[KEY_PHONE_NO].as< int>() == bob().phone_no);

  Employee_t employee;
  schemer::parse(bobEmployeeNode, &employee, &parse);

  BOOST_REQUIRE(bobEmployee == employee);
}

BOOST_AUTO_TEST_CASE(MapTest)
{
  using std::string;

  // SETTINGS //
  typedef string Key;
  typedef int Value;
  typedef std::map< Key, Value> Map;
  Map ORIGINAL_MAP;
  ORIGINAL_MAP["Bob"] = 32523;
  ORIGINAL_MAP["Alice"] = 632;
  ORIGINAL_MAP["Charles"] = 9532;

  schemer::ParseLog parse;

  YAML::Node mapNode;
  schemer::serialise(ORIGINAL_MAP, &mapNode);

  BOOST_FOREACH(Map::const_reference x, ORIGINAL_MAP)
  {
    BOOST_REQUIRE(mapNode[x.first]);
    BOOST_REQUIRE(mapNode[x.first].as< Value>() == x.second);
  }

  Map newMap;
  schemer::parse(mapNode, &newMap, &parse);

  BOOST_REQUIRE(ORIGINAL_MAP == newMap);
}

SCHEMER_HOMO_MAP(PersonInfo)
{
  element("Firstname")->defaultValue("Anon");
  element("Lastname")->defaultValue("Ymous");
}

BOOST_AUTO_TEST_CASE(MapDefaultValuesTest)
{
  // SETTINGS ///
  YAML::Node peopleNode;
  peopleNode["Firstname"] = YAML::Node();
  peopleNode["Lastname"] = YAML::Node();

  std::map< std::string, std::string> people;
  PersonInfo pInfo;
  pInfo.nodeToValue(peopleNode, &people, NULL);

  BOOST_REQUIRE(!people.empty());
  std::map< std::string, std::string>::const_iterator it = people.find(
      "Firstname");
  BOOST_REQUIRE(it->second == "Anon");
  it = people.find("Lastname");
  BOOST_REQUIRE(it->second == "Ymous");
}

typedef schemer::Map< schemer::Int, schemer::String> IdsType;

BOOST_AUTO_TEST_CASE(HomoMapTest)
{
  typedef std::map<int, std::string> IdsMap;
  IdsMap ids;
  ids[54654] = "Martin";
  ids[3867] = "Tom";
  ids[6874321] = "Alice";

  YAML::Node idsNode;
  static const IdsType IDS_TYPE;
  BOOST_REQUIRE(IDS_TYPE.valueToNode(ids, &idsNode));
  BOOST_FOREACH(IdsMap::const_reference x, ids)
  {
    BOOST_REQUIRE(idsNode[x.first]);
    BOOST_REQUIRE(idsNode[x.first].as< std::string>() == x.second);
  }

  std::map< int, std::string> idsFromNode;
  BOOST_REQUIRE(IDS_TYPE.nodeToValue(idsNode, &idsFromNode));
  BOOST_REQUIRE(ids == idsFromNode);
}

const Person_t &
bob()
{
  static Person_t bob;
  static bool initialised = false;
  if(!initialised)
  {
    bob.name = BOB_NAME;
    bob.age = BOB_AGE;
    bob.phone_no = BOB_PHONE_NO;
    initialised = true;
  }
  return bob;
}

