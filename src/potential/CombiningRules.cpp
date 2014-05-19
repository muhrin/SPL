/*
 * CombiningRules.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/CombiningRules.h"

#include "spl/SSLib.h"

#include <map>

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////

namespace spl {
namespace potential {

class Rules
{
  static std::map< CombiningRule::Value, std::string> map;
public:
  static std::string
  toString(const CombiningRule::Value c)
  {
    init();
    return Rules::map[c];
  }
  static CombiningRule::Value
  fromString(const std::string & s)
  {
    typedef std::pair< CombiningRule::Value, std::string> RulePair;
    init();

    BOOST_FOREACH(const RulePair & r, Rules::map)
    {
      if(s == r.second)
      {
        return r.first;
        std::cout << s << " == " << r.second;
      }
      else
        std::cout << s << " != " << r.second;
      std::cout << "\n";
    }
    return CombiningRule::NUM_RULES;
  }
private:
  Rules()
  {
  }
  static void
  init()
  {
    static bool initialised = false;
    if(!initialised)
    {
      Rules::map[CombiningRule::NONE] = "none";
      Rules::map[CombiningRule::LORENTZ] = "lorentz";
      Rules::map[CombiningRule::BERTHELOT] = "berthelot";
      Rules::map[CombiningRule::UHRIN_PICKARD] = "lj";
      initialised = true;
    }
  }
};
std::map< CombiningRule::Value, std::string> Rules::map;

std::ostream &
operator <<(std::ostream & os, const CombiningRule::Value & rule)
{
  os << Rules::toString(rule);
  return os;
}

std::istream &
operator >>(std::istream & is, CombiningRule::Value & rule)
{
  std::stringstream ss;
  ss << is;
  const CombiningRule::Value r = Rules::fromString(ss.str());
  if(r == CombiningRule::NUM_RULES) // Used to indicate malformed rule
    is.setstate(is.failbit);
  else
    rule = r;
  return is;
}

double
applyRule(const CombiningRule::Value rule, const double x1, const double x2)
{
  switch(rule)
  {
  case CombiningRule::BERTHELOT:
    return sqrt(x1 * x2);
  case CombiningRule::UHRIN_PICKARD:
    return sqrt(16 - (x1 + x2) * (x1 + x2));
  case CombiningRule::LORENTZ:
    return 0.5 * (x1 + x2);
  }

  SSLIB_ASSERT("Unknown combining rule supplied.");
  return 0.0;
}

}
}
