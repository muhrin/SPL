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

// DEFINES //////////////////////////////////////////////


// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx { namespace potential {

class IParameterisable
{
public:
	virtual ~IParameterisable() {}

	virtual const ::std::string & getName() const = 0;

	virtual size_t getNumParams() const = 0;
	virtual ::arma::vec getParams() const = 0;
	virtual void setParams(const ::arma::vec & params) = 0;

	virtual const ::std::string & getParamString() const = 0;
};

}}

#endif /* I_PARAMETERISABLE_H */
