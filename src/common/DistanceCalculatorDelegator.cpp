/*
 * DistanceCalculatorDelegator.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "spl/common/DistanceCalculatorDelegator.h"

#include "spl/common/ClusterDistanceCalculator.h"
#include "spl/common/OrthoCellDistanceCalculator.h"
#include "spl/common/UnitCell.h"
#include "spl/common/UniversalCrystalDistanceCalculator.h"

namespace spl {
namespace common {

DistanceCalculatorDelegator::DistanceCalculatorDelegator(Structure & structure) :
    myStructure(structure), myDelegate(new ClusterDistanceCalculator()), myDelegateType(
        CalculatorType::CLUSTER)
{
  // WARNING: Don't use structure here as it won't be initialised!!
}

void
DistanceCalculatorDelegator::setUnitCell(common::UnitCell * const unitCell)
{
  updateDelegate();
}

void
DistanceCalculatorDelegator::updateDelegate()
{
  UnitCell * const unitCell = myStructure.getUnitCell();

  if(unitCell == NULL)
    setDelegate(CalculatorType::CLUSTER);
  else
  {
    const UnitCell::LatticeSystem::Value latticeSystem =
        unitCell->getLatticeSystem(
            OrthoCellDistanceCalculator::VALID_ANGLE_TOLERANCE);
    if(latticeSystem == UnitCell::LatticeSystem::TETRAGONAL
        || latticeSystem == UnitCell::LatticeSystem::CUBIC
        || latticeSystem == UnitCell::LatticeSystem::ORTHORHOMBIC)
      setDelegate(CalculatorType::ORTHO_CELL);
    else
      setDelegate(CalculatorType::UNIVERSAL_CRYSTAL);
  }

  myDelegate->setUnitCell(unitCell);
}

bool
DistanceCalculatorDelegator::setDelegate(const CalculatorType::Value calcType)
{
  bool delegateChanged = false;
  if(myDelegateType != calcType)
  {
    if(calcType == CalculatorType::CLUSTER)
    {
      myDelegate.reset(new ClusterDistanceCalculator());
      delegateChanged = true;
    }
    else if(calcType == CalculatorType::UNIVERSAL_CRYSTAL)
    {
      myDelegate.reset(
          new UniversalCrystalDistanceCalculator(myStructure.getUnitCell()));
      delegateChanged = true;
    }
    else if(calcType == CalculatorType::ORTHO_CELL)
    {
      myDelegate.reset(
          new OrthoCellDistanceCalculator(myStructure.getUnitCell()));
      delegateChanged = true;
    }
    myDelegateType = calcType;
  }

  return delegateChanged;
}

} // namespace spl
} // namespace common
