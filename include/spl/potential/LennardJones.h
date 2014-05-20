/*
 * LennardJones.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

// DEFINES ////////////////////////////////////////////////

// INCLUDES ///////////////////////////////////////////////

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include "spl/common/Structure.h"
#include "spl/potential/CombiningRules.h"
#include "spl/potential/GenericPotentialEvaluator.h"
#include "spl/potential/IParameterisable.h"
#include "spl/potential/IPotential.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace potential {

// Generalised Leannard-Jones potential:
//
// U(r) = 4 epsilon * [(sigma/r)^m - (sigma/r)^n]
// TODO: Update with cutoff version
class LennardJones : public IPotential, public IParameterisable
{
public:
  struct Params
  {
    double epsilon;
    double sigma;
    double m;
    double n;
    double cutoff;
    double energyShift;
    double forceShift;
  };

  typedef std::map< SpeciesPair, Params> Interactions;
  typedef PotentialData DataType;

  static const unsigned int MAX_INTERACTION_VECTORS = 50000;
  static const unsigned int MAX_CELL_MULTIPLES = 500;

  LennardJones();

  // From Parameterisable ////////////////////////////////////////
  virtual std::vector< std::string>
  getParams() const;
  virtual bool
  setParams(const std::vector< std::string> & params,
      std::string * const errorMsg);
  virtual bool
  updateSpeciesDb(common::AtomSpeciesDatabase * const speciesDb) const;
  // End Parameterisable //////////////////////////////////////////

  // From IPotential /////////////
  virtual boost::optional< double>
  getSpeciesPairDistance(const SpeciesPair & pair) const;
  virtual boost::shared_ptr< IPotentialEvaluator>
  createEvaluator(const spl::common::Structure & structure) const;
  virtual IParameterisable *
  getParameterisable();
  // End from IPotential /////////

  bool
  evaluate(const common::Structure & structure, PotentialData & data) const;

  std::pair< Interactions::const_iterator, bool>
  addInteraction(const SpeciesPair & species, const double epsilon,
      const double sigma, const double m, const double n,
      const double cutoffFactor);

private:
  typedef GenericPotentialEvaluator< LennardJones> Evaluator;

  static const double MIN_SEPARATION_SQ;

  std::pair< double, double>
  evaluate(const double r, const Params & params) const;

  void
  init();
  void
  applyCombiningRule();
  double
  equilibriumSeparation(const Params & params) const;
  Params
  getParams(const double epsilon, const double sigma, const double m,
      const double n, const double cutoffFactor) const;

  CombiningRule::Value myEpsilonCombining;
  CombiningRule::Value mySigmaCombining;

  Interactions myInteractions;
};

}
}

#endif /* LENNARD_JONES_H */

