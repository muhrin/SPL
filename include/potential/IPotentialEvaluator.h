/*
 * IPotentialEvaluator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_POTENTIAL_EVALUATOR_H
#define I_POTENTIAL_EVALUATOR_H

// INCLUDES /////////////////////////////////////////////
#include "PotentialData.h"

namespace sstbx {
namespace potential {

// FORWARD DECLARATIONS ////////////////////////////////////
class IPotential;
struct PotentialData;

class IPotentialEvaluator {
public:

  typedef ::std::pair<PotentialData *, bool> EvalResult;

  virtual PotentialData & getData() = 0;

	virtual EvalResult evalPotential() = 0;

  virtual const IPotential & getPotential() const = 0;

};

}}

#endif /* I_POTENTIAL_EVALUATOR_H */
