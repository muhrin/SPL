/*
 * DistanceCalculatorDelegator.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/DistanceCalculatorDelegator.h"

#include "common/ClusterDistanceCalculator.h"
#include "common/OrthoCellDistanceCalculator.h"
#include "common/UnitCell.h"
#include "common/UniversalCrystalDistanceCalculator.h"

namespace sstbx {
namespace common {

DistanceCalculatorDelegator::DistanceCalculatorDelegator(const Structure & structure):
DistanceCalculator(structure),
myDelegate(new ClusterDistanceCalculator(structure)),
myDelegateType(CalculatorType::CLUSTER)
{
  // WARNING: Don't use structure here as it won't be initialised!!
}

void DistanceCalculatorDelegator::unitCellChanged()
{
  updateDelegate();
}

void DistanceCalculatorDelegator::updateDelegate()
{
  const UnitCell * const unitCell = myStructure.getUnitCell();

  bool delegateChanged = false;
  if(unitCell == NULL)
  {
    delegateChanged = setDelegate(CalculatorType::CLUSTER);
  }
  else
  {
    const UnitCell::LatticeSystem::Value latticeSystem = unitCell->getLatticeSystem(OrthoCellDistanceCalculator::VALID_ANGLE_TOLERANCE);
    if(latticeSystem == UnitCell::LatticeSystem::TETRAGONAL ||
      latticeSystem == UnitCell::LatticeSystem::CUBIC ||
      latticeSystem == UnitCell::LatticeSystem::ORTHORHOMBIC)
    {
      delegateChanged = setDelegate(CalculatorType::ORTHO_CELL);
    }
    else
    {
      delegateChanged = setDelegate(CalculatorType::UNIVERSAL_CRYSTAL);
    }
  }

  if(!delegateChanged)
    myDelegate->unitCellChanged();
}

bool DistanceCalculatorDelegator::setDelegate(const CalculatorType::Value calcType)
{
  bool delegateChanged = false;
  if(myDelegateType != calcType)
  {
    if(calcType == CalculatorType::CLUSTER)
    {
      myDelegate.reset(new ClusterDistanceCalculator(myStructure));
      delegateChanged = true;
    }
    else if(calcType == CalculatorType::UNIVERSAL_CRYSTAL)
    {
      myDelegate.reset(new UniversalCrystalDistanceCalculator(myStructure));
      delegateChanged = true;
    }
    else if(calcType == CalculatorType::ORTHO_CELL)
    {
      myDelegate.reset(new OrthoCellDistanceCalculator(myStructure));
      delegateChanged = true;
    }
    myDelegateType = calcType;
  }

  return delegateChanged;
}

} // namespace sstbx
} // namespace common
