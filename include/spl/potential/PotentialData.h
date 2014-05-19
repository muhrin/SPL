/*
 * PotentialData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef POTENTIAL_DATA_H
#define POTENTIAL_DATA_H

// INCLUDES /////////////////////////////////////////////
#include <cstring>

#include <armadillo>


// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace potential {

struct PotentialData
{
  PotentialData();

  double internalEnergy;
  arma::mat forces;
  arma::mat33 stressMtx;

  void
  reset();
};

}
}

#endif /* POTENTIAL_DATA_H */
