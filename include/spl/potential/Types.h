/*
 * Types.h
 *
 *  Created on: Aug 30, 2012
 *      Author: Martin Uhrin
 */

#ifndef POTENTIAL__TYPES_H
#define POTENTIAL__TYPES_H

// INCLUDES ////////////
#include "spl/SSLib.h"

namespace spl {
namespace potential {

// FORWARD DECLARES ////////////
class GeomOptimiser;
class IPotential;

typedef UniquePtr<IPotential>::Type IPotentialPtr;
typedef UniquePtr<GeomOptimiser>::Type IGeomOptimiserPtr;

}
}

#endif /* POTENTIAL__TYPES_H */
