/*
 * AtomsFormula.h
 *
 *  Created on: Jul 20, 2013
 *      Author: Martin Uhrin
 */

#ifndef ATOM_FORMULA_H
#define ATOM_FORMULA_H

// INCLUDES ///////////////////////////////////
#include "spl/SSLib.h"

#include <map>
#include <ostream>
#include <string>

namespace spl {
namespace common {

// FORWARD DECLARES ///////////////////////////

class AtomsFormula
{
  typedef std::map< std::string, int> Formula;
public:
  typedef Formula::value_type Entry;
  typedef Formula::value_type value_type;
  typedef Formula::const_reference const_reference;
  typedef Formula::const_iterator const_iterator;
  typedef const_iterator iterator; // Only const iteration allowed
  typedef std::pair< int, int> Fraction;

  AtomsFormula()
  {
  }
  explicit
  AtomsFormula(const std::string & species);
  AtomsFormula(const std::string & species, const int number);

  bool
  fromString(const std::string & str);

  bool
  isEmpty() const;
  int
  numSpecies() const;
  int
  total() const;

  unsigned int
  reduce();

  bool
  operator ==(const AtomsFormula & rhs) const;
  bool
  operator <(const AtomsFormula & rhs) const;

  int &
  operator [](const std::string & species);
  AtomsFormula &
  operator +=(const AtomsFormula rhs);
  AtomsFormula &
  operator *=(const int timesBy);

  bool
  remove(const AtomsFormula & toRemove);
  bool
  remove(const AtomsFormula & toRemove, const int num);

  const_iterator
  begin() const;
  const_iterator
  end() const;

  bool
  contains(const std::string & species) const;
  bool
  contains(const AtomsFormula & formula) const;
  int
  numberOf(const std::string & species) const;
  Fraction
  numberOf(const Entry & entry) const;
  int
  wholeNumberOf(const AtomsFormula & formula) const;
  int
  numMultiples(const AtomsFormula & formula) const;

  void
  clear();

  std::string
  toString() const;
  void
  print(std::ostream & os) const;

  bool
  simplify(std::pair< int, int> & fraction) const;

private:
  static const Fraction ZERO;
  static const Fraction MAX;

  Fraction
  min(const Fraction & f1, const Fraction & f2) const;

  Formula myFormula;
};

std::ostream &
operator <<(std::ostream & os, const AtomsFormula & formula);

}
}

#endif /* ATOM_FORMULA_H */
