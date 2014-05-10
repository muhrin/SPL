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

#include "spl/common/Structure.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace potential {

struct PotentialData
{
  PotentialData();
  explicit
  PotentialData(const spl::common::Structure & structure);

  ::std::size_t numParticles;
  double internalEnergy;
  ::arma::mat pos;
  ::arma::mat forces;
  ::arma::mat33 stressMtx;
};

}
}

#endif /* POTENTIAL_DATA_H */
