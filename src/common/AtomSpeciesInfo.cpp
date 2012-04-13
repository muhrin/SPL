/*
 * AtomSpeciesInfo.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "common/AtomSpeciesInfo.h"


namespace sstbx { namespace common {

const ::std::string & AtomSpeciesInfo::getName() const
{
	return myName;
}

void AtomSpeciesInfo::setName(const ::std::string & name)
{
	myName = name;
}

const ::std::string & AtomSpeciesInfo::getSymbol() const
{
	return mySymbol;
}

void AtomSpeciesInfo::setSymbol(const ::std::string & symbol)
{
	mySymbol = symbol;
}

}}