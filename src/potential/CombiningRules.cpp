/*
 * CombiningRules.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/CombiningRules.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace potential {

bool
applyEnergyRule(arma::mat & energyParams, const CombiningRule::Value rule)
{
  if(!energyParams.is_square())
    return false;

  if(rule == CombiningRule::BERTHELOT
      || rule == CombiningRule::LORENTZ_BERTHELOT)
  {
    // Apply the Berthelot combining rule
    for(size_t i = 0; i < energyParams.n_rows - 1; ++i)
    {
      for(size_t j = i + 1; j < energyParams.n_cols; ++j)
      {
        energyParams(i, j) = energyParams(j, i) = std::sqrt(
            energyParams(i, i) * energyParams(j, j));
      }
    }
  }
  else if(rule == CombiningRule::UHRIN_PICKARD)
  {
    double sum = 0.0;
    // Apply the Berthelot combining rule
    for(size_t i = 0; i < energyParams.n_rows - 1; ++i)
    {
      for(size_t j = i + 1; j < energyParams.n_cols; ++j)
      {
        sum = energyParams(i, i) + energyParams(j, j);
        energyParams(i, j) = energyParams(j, i) = std::sqrt(16.0 - sum * sum);
      }
    }
  }

  return false;
}

bool
applySizeRule(arma::mat & sizeParams, const CombiningRule::Value rule)
{
  if(!sizeParams.is_square())
    return false;

  if(rule == CombiningRule::LORENTZ || rule == CombiningRule::LORENTZ_BERTHELOT)
  {
    for(size_t i = 0; i < sizeParams.n_rows - 1; ++i)
    {
      for(size_t j = i + 1; j < sizeParams.n_cols; ++j)
        sizeParams(i, j) = sizeParams(j, i) = 0.5
            * (sizeParams(i, i) + sizeParams(j, j));
    }
    return true;
  }

  return false;
}

CombiningRule::Value
getRuleFromString(const std::string str)
{
  CombiningRule::Value rule = CombiningRule::NONE;

  if(str == "lorentz")
    rule = CombiningRule::LORENTZ;
  else if(str == "berthelot")
    rule = CombiningRule::BERTHELOT;
  else if(str == "lorentzBerthelot")
    rule = CombiningRule::LORENTZ_BERTHELOT;
  else if(str == "uhrinPickard")
    rule = CombiningRule::UHRIN_PICKARD;
  else if(str == "unknown")
    rule = CombiningRule::UNKNOWN;

  return rule;
}

::std::string
getStringFromRule(const CombiningRule::Value rule)
{
  std::string ruleString;

  switch(rule)
  {
  case CombiningRule::NONE:
  {
    ruleString = "none";
    break;
  }
  case CombiningRule::LORENTZ:
  {
    ruleString = "lorentz";
    break;
  }
  case CombiningRule::BERTHELOT:
  {
    ruleString = "berthelot";
    break;
  }
  case CombiningRule::LORENTZ_BERTHELOT:
  {
    ruleString = "lorentzBerthelot";
    break;
  }
  case CombiningRule::UHRIN_PICKARD:
  {
    ruleString = "uhrinPickard";
    break;
  }
  default:
  {
    ruleString = "unknown";
    break;
  }
  }
  return ruleString;
}

}
}
