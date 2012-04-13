/*
 * AtomSpeciesInfo.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_SPECIES_INFO_H
#define ATOM_SPECIES_INFO_H

// INCLUDES /////////////////////////////////////////////

#include "AtomSpeciesId.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx { namespace common {

class AtomSpeciesInfo
{
public:

	virtual ~AtomSpeciesInfo() {}

	virtual const ::std::string & getName() const;
	virtual void setName(const ::std::string & name);

	virtual const ::std::string & getSymbol() const;
	virtual void setSymbol(const ::std::string & symbol);

protected:

	::std::string myName;
	::std::string mySymbol;

};

}}

#endif /* ATOM_SPECIES_INFO_H */
