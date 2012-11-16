/*
 * IParameterisable.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_PARAMETERISABLE_H
#define I_PARAMETERISABLE_H

// INCLUDES /////////////////////////////////////////////
#include <armadillo>

#include <utility>

// DEFINES //////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx { namespace potential {

class IParameterisable
{
public:
	virtual ~IParameterisable() {}

  /**
  /* Returns the number of parameters that this potential has.
  /*/
	virtual size_t getNumParams() const = 0;

  /**
  /* Get the current parameters vector for this potential.
  /**/
	virtual arma::vec getParams() const = 0;

  /**
  /* Set the parameters for this potential.
  /**/
	virtual void setParams(const ::arma::vec & params) = 0;

	virtual const std::string & getParamString() const = 0;

  virtual std::pair<arma::vec, bool> getParamsFromString(const std::string & str) const = 0;
};

}}

#endif /* I_PARAMETERISABLE_H */
