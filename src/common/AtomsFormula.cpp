/*
 * AtomsFormula.cpp
 *
 *  Created on: Jul 20, 2013
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "spl/common/AtomsFormula.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "spl/SSLibAssert.h"
#include "spl/math/NumberAlgorithms.h"

namespace spl {
namespace common {

AtomsFormula::AtomsFormula(const ::std::string & species)
{
  myFormula[species] = 1;
}

AtomsFormula::AtomsFormula(const ::std::string & species, const int number)
{
  myFormula[species] = number;
}

bool AtomsFormula::fromString(const ::std::string & str)
{
  static const boost::regex FORM_EXPRESSION("([[:upper:]][[:lower:]]*)([[:digit:]]*)");

  if(str.empty())
    return false;

  ::std::string::const_iterator start = str.begin();
  const ::std::string::const_iterator end = str.end();
  boost::match_results< ::std::string::const_iterator> match;
  boost::match_flag_type flags = boost::match_default;
  while(::boost::regex_search(start, end, match, FORM_EXPRESSION, flags))
  {
    // Get the species
    Entry entry(::std::string(match[1].first, match[1].second), 1);

    // Get the number
    ::std::string numStr(match[2].first, match[2].second);
    if(!numStr.empty())
    {
      try
      {
        entry.second = ::boost::lexical_cast<int>(numStr);
      }
      catch(const ::boost::bad_lexical_cast & /*e*/)
      {
        return false;
      }
    }
    myFormula.insert(entry);

    // update search position:
    start = match[0].second;
    // update flags:
    flags |= boost::match_prev_avail;
    flags |= boost::match_not_bob;
  }

  return !isEmpty();
}

unsigned int AtomsFormula::reduce()
{
  // Divide the counts by the greatest common divisor
  ::std::vector<unsigned int> counts;
  BOOST_FOREACH(Formula::const_reference e, myFormula)
    counts.push_back(e.second);
  const unsigned int gcd = math::greatestCommonDivisor(counts);
  BOOST_FOREACH(Formula::reference e, myFormula)
    e.second /= static_cast<int>(gcd);
  return gcd;
}

bool AtomsFormula::isEmpty() const
{
  return myFormula.empty();
}

int AtomsFormula::numSpecies() const
{
  return myFormula.size();
}

bool AtomsFormula::operator ==(const AtomsFormula & rhs) const
{
  BOOST_FOREACH(Formula::const_reference e, myFormula)
  {
    if(rhs.numberOf(e.first) != e.second)
      return false;
  }
  return true;
}

bool AtomsFormula::operator <(const AtomsFormula & rhs) const
{
  for(Formula::const_iterator it = myFormula.begin(), end = myFormula.end(),
      rhsIt = rhs.myFormula.begin(), rhsEnd = rhs.myFormula.end();
      it != end && rhsIt != rhsEnd; ++it, ++rhsIt)
  {
    if(it->first < rhsIt->first)
      return true;
    else if(it->first > rhsIt->first)
      return false;
    else if(it->second < rhsIt->second)
      return true;
    else if(it->second > rhsIt->second)
      return false;
  }
  // So far they have been the same, maybe they are different lengths?
  return myFormula.size() < rhs.myFormula.size();
}

AtomsFormula & AtomsFormula::operator +=(const AtomsFormula rhs)
{
  BOOST_FOREACH(Formula::const_reference e, rhs)
  {
    myFormula[e.first] += e.second;
  }
  return *this;
}

bool AtomsFormula::remove(const AtomsFormula & toRemove)
{
  return remove(toRemove, 1);
}

bool AtomsFormula::remove(const AtomsFormula & toRemove, const int numToRemove)
{
  // Find out how many of those to remove we have
  const int num = wholeNumberOf(toRemove);

  // Check we have that formula to remove and that we don't have a fractional number of them
  if(num == 0 || numToRemove > num)
    return false;

  Formula::iterator it;
  BOOST_FOREACH(Formula::const_reference e, toRemove)
  {
    it = myFormula.find(e.first);
    it->second -= numToRemove * e.second;
    if(it->second == 0)
      myFormula.erase(it);
  }
  return true;
}

int & AtomsFormula::operator [](const ::std::string & species)
{
  return myFormula[species];
}

AtomsFormula::const_iterator AtomsFormula::begin() const
{
  return myFormula.begin();
}

AtomsFormula::const_iterator AtomsFormula::end() const
{
  return myFormula.end();
}

bool AtomsFormula::contains(const ::std::string & species) const
{
  return myFormula.find(species) != myFormula.end();
}

bool AtomsFormula::contains(const AtomsFormula & formula) const
{
  return wholeNumberOf(formula) != 0;
}

int AtomsFormula::numberOf(const std::string & species) const
{
  const Formula::const_iterator it = myFormula.find(species);
  if(it == myFormula.end())
    return 0;

  return it->second;
}

AtomsFormula::Fraction AtomsFormula::numberOf(const Entry & entry) const
{
  return Fraction(numberOf(entry.first), entry.second);
}

int AtomsFormula::wholeNumberOf(const AtomsFormula & formula) const
{
  if(formula.isEmpty() || formula.numSpecies() > numSpecies())
    return 0;

  Fraction currentFrac, min = MAX;
  BOOST_FOREACH(Formula::const_reference e, formula)
  {
    currentFrac = numberOf(e);
    if(currentFrac.first < currentFrac.second)
      return 0;
    else
      min = this->min(min, currentFrac);
  }

  // Integer division will round down for us
  return min.first / min.second;
}

int AtomsFormula::numMultiples(const AtomsFormula & formula) const
{
  if(numSpecies() != formula.numSpecies())
    return 0;

  int multiple = 0;
  Fraction currentFrac;
  BOOST_FOREACH(Formula::const_reference e, formula)
  {
    currentFrac = numberOf(e);
    simplify(currentFrac);
    if(currentFrac.first == 0 || currentFrac.second != 1)
      return 0;
    else if(multiple == 0)
      multiple = currentFrac.first;
    else if(currentFrac.first != multiple)
      return 0;
  }

  return multiple;
}

::std::string AtomsFormula::toString() const
{
  ::std::stringstream ss;
  BOOST_FOREACH(Formula::const_reference e, myFormula)
  {
    ss << e.first;
    if(e.second != 1)
      ss << e.second;
  }
  return ss.str();
}

void AtomsFormula::print(::std::ostream & os) const
{
  os << toString();
}

bool AtomsFormula::simplify(::std::pair<int, int> & fraction) const
{
  if(fraction.first == 0 || fraction.second == 0)
    return false;

  const int gcd = math::greatestCommonDivisor(fraction.first, fraction.second);
  if(gcd == 1)
    return false;

  fraction.first = fraction.first / gcd;
  fraction.second = fraction.second / gcd;
  return true;
}

const AtomsFormula::Fraction AtomsFormula::ZERO = AtomsFormula::Fraction(0, 1);
const AtomsFormula::Fraction AtomsFormula::MAX = AtomsFormula::Fraction(::std::numeric_limits<int>::max(), 1);

AtomsFormula::Fraction AtomsFormula::min(const Fraction & f1, const Fraction & f2) const
{
  return static_cast<long>(f1.first) * static_cast<long>(f2.second)
      < static_cast<long>(f1.second) * static_cast<long>(f2.first) ? f1 : f2;
}

::std::ostream & operator <<(::std::ostream & os, const AtomsFormula & formula)
{
  formula.print(os);
  return os;
}

}
}
