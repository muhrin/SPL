/*
 * UnitCellFactory.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "UnitCellFactory.h"

#include "NonOrthorhombicUnitCell.h"
#include "OrthorhombicUnitCell.h"

namespace sstbx { namespace common {

UnitCellFactory * UnitCellFactory::instance = 0;

AbstractFmidCell<double> * UnitCellFactory::createCell(
	const double a, const double b, const double c,
	const double alpha, const double beta, const double gamma) const
{
	AbstractFmidCell<double> * cell = 0;
	if(alpha == 90.0 && beta == 90.0 && gamma == 90.0)
	{
		cell = new OrthorhombicUnitCell<double>(a, b, c, alpha, beta, gamma);
	}
	else
	{
		cell = new NonOrthorhombicUnitCell<double>(a, b, c, alpha, beta, gamma);
	}
	return cell;
}

const UnitCellFactory * UnitCellFactory::inst()
{
	if(instance == 0)
	{
		instance = new UnitCellFactory();
	}
	return instance;
}

}}