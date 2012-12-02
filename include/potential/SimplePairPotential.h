/*
 * SimplePairPotential.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_PAIR_POTENTIAL_H
#define SIMPLE_PAIR_POTENTIAL_H

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

// Local includes
#include "common/AtomSpeciesDatabase.h"
#include "common/Structure.h"
#include "common/Utils.h"
#include "potential/GenericPotentialEvaluator.h"
#include "potential/IParameterisable.h"
#include "potential/IPotential.h"
#include "potential/SimplePairPotentialData.h"


// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace potential {

class SimplePairPotential :	public IPotential, public IParameterisable
{
public:

  /**
  /* A list from 0 to N of the species that each row (and column) of the parameter
  /* matrices corresponds to.  The entries should be unique.
  /*                      Na Cl
  /* epsilon_{ij} = Na (  1  0.5 )
  /*                CL ( 0.5  1  )
  /* speciesMap(0 => Na, 1 => Cl)
  /**/
  typedef SimplePairPotentialData::SpeciesList  SpeciesList;
  typedef SimplePairPotentialData DataType;

  /**
  /* Combining rules for setting off-diagonal length/energy scale terms. See
  /* http://www.sklogwiki.org/SklogWiki/index.php/Combining_rules
  /* for good reference.
  /* If a rule is being used it will overwrite any off diagonal parameters.
  /**/
  enum CombiningRule
  {
    NONE,
    LORENTZ,
    BERTHELOT,
    LORENTZ_BERTHELOT,
    CUSTOM
  };

  static const unsigned int MAX_INTERACTION_VECTORS = 5000;//10000;
  static const unsigned int MAX_CELL_MULTIPLES = 500;

	SimplePairPotential(
    common::AtomSpeciesDatabase & atomSpeciesDb,
		const size_t 				          numSpecies,
    const SpeciesList &           speciesList,
		const ::arma::mat &		        epsilon,
		const ::arma::mat &		        sigma,
		const double 			            cutoffFactor,
		const ::arma::mat &	          beta,
		const double  			          m,
		const double 	  		          n,
    const CombiningRule           combiningRule = NONE);

	virtual const ::std::string & getName() const;

	// From IParameterisable ////////////////////////////////////////

	virtual size_t getNumParams() const;
	virtual const ::std::string & getParamString() const;
	virtual ::arma::vec getParams() const;
	virtual void setParams(const ::arma::vec & params);
  virtual std::pair<arma::vec, bool>
    getParamsFromString(const std::string & str) const;

	// End IParameterisable //////////////////////////////////////////


  // From IPotential /////////////
  virtual ::boost::optional<double> getPotentialRadius(const ::sstbx::common::AtomSpeciesId::Value id) const;
  virtual ::boost::shared_ptr< IPotentialEvaluator > createEvaluator(const sstbx::common::Structure & structure) const;
  // End from IPotential /////////

  bool evaluate(const common::Structure & structure, SimplePairPotentialData & data) const;

private:

  static const double RADIUS_FACTOR;
  static const double MIN_SEPARATION_SQ;

  typedef GenericPotentialEvaluator<SimplePairPotential > Evaluator;

	void initCutoff(const double cutoff);

  void applyCombiningRule();

	void resetAccumulators(SimplePairPotentialData & data) const;

  void updateSpeciesDb();

  common::AtomSpeciesDatabase & myAtomSpeciesDb;

	const ::std::string		myName;

	mutable ::std::string	myParamString;

	/** Potential parameters */
	size_t					myNumSpecies;
  SpeciesList     mySpeciesList;
	::arma::mat			myEpsilon;
	::arma::mat			mySigma;
	::arma::mat			myBeta;
  const double    myCutoffFactor;

	/** The powers of the sigma/r terms in the potential */
	double				myN, myM;

  CombiningRule myCombiningRule;

  ::arma::mat 	rCutoff;
  ::arma::mat 	rCutoffSq;
  ::arma::mat 	eShift;
  ::arma::mat 	fShift;
};


}
}

#endif /* SIMPLE_PAIR_POTENTIAL_H */

