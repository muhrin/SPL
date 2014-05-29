/*
 * Utility.h
 *
 *  Created on: May 22, 2014
 *      Author: Martin Uhrin
 */

#ifndef SPL__BUILD_CELL__UTILITY_H_
#define SPL__BUILD_CELL__UTILITY_H_

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <boost/optional.hpp>

#include "spl/common/AtomsFormula.h"
#include "spl/common/AtomSpeciesDatabase.h"

// DEFINITION ///////////////////////

namespace spl {
namespace build_cell {

double
naturalVolume(const common::AtomsFormula & atoms,
    const common::AtomSpeciesDatabase & speciesDb, const bool periodic);

}
}

#endif /* SPL__BUILD_CELL__UTILITY_H_ */
