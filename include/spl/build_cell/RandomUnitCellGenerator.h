/*
 * RandomUnitCellGenerator.h
 *
 *  Created on: Aug 30, 2011
 *      Author: Martin Uhrin
 */

#ifndef RANDOM_UNIT_CELL_GENERATOR_H
#define RANDOM_UNIT_CELL_GENERATOR_H

// INCLUDES ////////////

#include "spl/OptionalTypes.h"
#include "spl/build_cell/IUnitCellGenerator.h"
#include "spl/utility/IndexingEnums.h"

// FORWARD DECLARATIONS ///////

namespace spl {
namespace build_cell {

class RandomUnitCellGenerator : public IUnitCellGenerator
{
public:
  typedef ::std::pair< double, bool> ParameterValueAndSpecific;

  static const double DEFAULT_MIN_ANGLE;
  static const double DEFAULT_MAX_ANGLE;
  static const double DEFAULT_MIN_LENGTH;
  static const double DEFAULT_MAX_LENGTH;
  static const double DEFAULT_TARGET_VOLUME;
  static const double DEFAULT_VOLUME_DELTA; // Volume can be +/- this value as a percentage of the target
  static const double DEFAULT_MAX_LENGTH_RATIO;
  static const double DEFAULT_BULK_CONTENTS_MULTIPLIER;
  static const double DEFAULT_CLUSTER_CONTENTS_MULTIPLIER;

  ParameterValueAndSpecific
  getMin(const size_t param) const;
  ParameterValueAndSpecific
  getMax(const size_t param) const;

  void
  setMin(const size_t param, const OptionalDouble min = OptionalDouble());
  void
  setMax(const size_t param, const OptionalDouble max = OptionalDouble());

  void
  setMinLengths(const OptionalDouble length = OptionalDouble());
  void
  setMaxLengths(const OptionalDouble length = OptionalDouble());

  void
  setMinAngles(const OptionalDouble angle = OptionalDouble());
  void
  setMaxAngles(const OptionalDouble angle = OptionalDouble());

  void
  setTargetVolume(const OptionalDouble volume = OptionalDouble());
  void
  setVolumeDelta(const OptionalDouble delta = OptionalDouble());
  void
  setContentsMultiplier(const OptionalDouble contentsMultiplier =
      OptionalDouble());

  void
  setMaxLengthRatio(const OptionalDouble maxLengthRatio = OptionalDouble());
  ParameterValueAndSpecific
  getMaxLengthRatio() const;

  // From IUnitCellGenerator ////
  virtual GenerationOutcome
  generateCell(common::UnitCellPtr & cellOut, const bool structureIsCluster =
      false) const;
  virtual GenerationOutcome
  generateCell(common::UnitCellPtr & cellOut,
      const StructureContents & contents,
      const bool structureIsCluster = false) const;

  virtual IUnitCellGeneratorPtr
  clone() const;
  // End from IUnitCellGenerator //////

private:
  typedef ::std::pair< OptionalDouble, OptionalDouble> OrderedPair;
  typedef ::std::pair< size_t, size_t> OrderedPairIndex;
  typedef ::std::pair< double, double> VolAndDelta;

  inline bool
  isLength(const size_t param) const
  {
    return param <= utility::cell_params_enum::C;
  }

  double
  generateParameter(const size_t param) const;
  GenerationOutcome
  generateLatticeParameters(common::UnitCellPtr & cellOut) const;

  void
  generateLengths(double (&latticeParams)[6]) const;

  double
  generateVolume(const VolAndDelta & volAndDelta) const;

  OrderedPairIndex
  getOrderedPairLengths(const double (&latticeParams)[6]) const;

  bool
  areParametersValid(const double (&latticeParams)[6]) const;
  bool
  cellFullySpecified() const;
  VolAndDelta
  generateVolumeParams(const double currentVolume, const bool isCluster,
      const StructureContents * const structureContents = NULL) const;

  /** An array of the optional min/max values of the unit cell lattice parameters. */
  OrderedPair myParameters[6];

  OptionalDouble myMinLength;
  OptionalDouble myMaxLength;
  OptionalDouble myMinAngle;
  OptionalDouble myMaxAngle;
  OptionalDouble myTargetVolume;
  OptionalDouble myContentsMultiplier;
  OptionalDouble myVolumeDelta;
  OptionalDouble myMaxLengthRatio;
};

}
}

#endif /* RANDOM_UNIT_CELL_GENERATOR_H */
