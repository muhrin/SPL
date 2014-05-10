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
#include "spl/potential/LennardJonesData.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace potential {

class LennardJones : public IPotential, public IParameterisable
{
public:

  // A list from 0 to N of the species that each row (and column) of the parameter
  // matrices corresponds to.  The entries should be unique.
  //                      Na Cl
  // epsilon_{ij} = Na (  1  0.5 )
  //                CL ( 0.5  1  )
  // speciesMap(0 => Na, 1 => Cl)
  typedef SimplePairPotentialData::SpeciesList SpeciesList;
  typedef SimplePairPotentialData DataType;

  static const unsigned int MAX_INTERACTION_VECTORS = 5000;
  static const unsigned int MAX_CELL_MULTIPLES = 500;

  static unsigned int
  numParams(const unsigned int numSpecies);

  LennardJones(const SpeciesList & speciesList, const arma::mat & epsilon,
      const arma::mat & sigma, const double cutoffFactor,
      const arma::mat & beta, const double m, const double n,
      const CombiningRule::Value combiningRule = CombiningRule::NONE);

  virtual const std::string &
  getName() const;

  // From IParameterisable ////////////////////////////////////////
  virtual size_t
  getNumParams() const;
  virtual IParameterisable::PotentialParams
  getParams() const;
  virtual void
  setParams(const IParameterisable::PotentialParams & params);
  virtual bool
  updateSpeciesDb(common::AtomSpeciesDatabase * const speciesDb) const;
  // End IParameterisable //////////////////////////////////////////

  // From IPotential /////////////
  virtual boost::optional< double>
  getPotentialRadius(const spl::common::AtomSpeciesId::Value id) const;
  virtual boost::optional< double>
  getSpeciesPairDistance(common::AtomSpeciesId::Value s1,
      common::AtomSpeciesId::Value s2) const;
  virtual boost::shared_ptr< IPotentialEvaluator>
  createEvaluator(const spl::common::Structure & structure) const;
  virtual IParameterisable *
  getParameterisable();
  // End from IPotential /////////

  bool
  evaluate(const common::Structure & structure,
      SimplePairPotentialData & data) const;

private:
  typedef GenericPotentialEvaluator< LennardJones> Evaluator;

  static const double EQUILIBRIUM_FACTOR;
  static const double MIN_SEPARATION_SQ;

  void
  init();
  void
  initCutoff();
  void
  applyCombiningRule();
  void
  resetAccumulators(SimplePairPotentialData & data) const;
  void
  updateEquilibriumSeparations();

  // Calculate energy
  inline double
  U(const size_t i, const size_t j, const double r) const
  {
    return 4.0 * myEpsilon(i, j)
        * (::std::pow(mySigma(i, j) / r, myM)
            - myBeta(i, j) * std::pow(mySigma(i, j) / r, myN));
  }
  // Calculate force
  inline double
  F(const size_t i, const size_t j, const double r) const
  {
    return 4.0 * myEpsilon(i, j) / r
        * (myM * std::pow(mySigma(i, j) / r, myM)
            - myBeta(i, j) * myN * std::pow(mySigma(i, j) / r, myN));
  }

  const std::string myName;

  /** Potential parameters */
  size_t myNumSpecies;
  const SpeciesList mySpeciesList;
  arma::mat myEpsilon;
  arma::mat mySigma;
  arma::mat myBeta;
  const double myCutoffFactor;

  /** The powers of the sigma/r terms in the potential */
  double myM, myN;

  CombiningRule::Value myCombiningRule;

  arma::mat rCutoff;
  arma::mat rCutoffSq;
  arma::mat eShift;
  arma::mat fShift;
  arma::mat myEquilibriumSeps;
};

}
}

#endif /* LENNARD_JONES_H */

