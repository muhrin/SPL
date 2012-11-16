/*
 * TpsdGeomOptimiser.h
 *
 * Two-point Step Size Gradient Methods - Barzilai and Borwein
 * IMA Journal of Numerical Analysis (1988) 8, 141-148
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef TPSD_GEOM_OPTIMISER_H
#define TPSD_GEOM_OPTIMISER_H

// INCLUDES /////////////////////////////////////////////
#include <limits>

#include <armadillo>

#include "potential/IGeomOptimiser.h"
#include "potential/IPotential.h"
#include "potential/IPotentialEvaluator.h"

#include "common/Structure.h"
#include "common/Types.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace common {
class UnitCell;
}
namespace potential {

class TpsdGeomOptimiser : public IGeomOptimiser
{
public:

	static const unsigned int DEFAULT_MAX_STEPS;
	static const double	DEFAULT_TOLERANCE;
  static const double DEFAULT_MIN_NORM_VOLUME;

	TpsdGeomOptimiser(
		const IPotential & potential,
		const size_t maxSteps = DEFAULT_MAX_STEPS,
		const double tolerance = DEFAULT_TOLERANCE,
    const double minNormVolume = DEFAULT_MIN_NORM_VOLUME);

	// IGeomOptimiser interface //////////////////////////////
  virtual const IPotential * getPotential() const;

	virtual bool optimise(
    ::sstbx::common::Structure & structure,
    const common::OptionalConstMat33 externalPressure = common::OptionalConstMat33()) const;

	virtual bool optimise(
		::sstbx::common::Structure &  structure,
    PotentialData & data,
    const common::OptionalConstMat33 externalPressure = common::OptionalConstMat33()) const;

	// End IGeomOptimiser interface

	bool optimise(
    common::Structure &   structure,
    IPotentialEvaluator & evaluator,
		const size_t          maxSteps,
		const double          eTol,
    const common::OptionalConstMat33 externalPressure = common::OptionalConstMat33()) const;

	bool optimise(
    common::Structure &   structure,
    common::UnitCell &    unitCell,
    IPotentialEvaluator & evaluator,
		const size_t          maxSteps,
		const double          eTol,
    const common::OptionalConstMat33 externalPressure = common::OptionalConstMat33()) const;

private:

  static const unsigned int CHECK_CELL_EVERY_N_STEPS;
  static const double CELL_MIN_NORM_VOLUME;
  static const double CELL_MAX_ANGLE_SUM;
  static const double MAX_STEPSIZE;

  bool cellReasonable(const common::UnitCell & unitCell) const;

	const IPotential & myPotential;

	const size_t myMaxSteps;
	const double myTolerance;
  const double myMinNormVolume;
};

}
}

#endif /* TPSD_GEOM_OPTIMISER_H */
