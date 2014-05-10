/*
 * CombiningRules.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef COMBINING_RULES_H
#define COMBINING_RULES_H

// INCLUDES /////////////////////////////////////////////
#include <armadillo>

// FORWARD DECLARATIONS ////////////////////////////////////


// DEFINITION //////////////////////////////////////////////

namespace spl {
namespace potential {


// Combining rules for setting off-diagonal length/energy scale terms. See
// http://www.sklogwiki.org/SklogWiki/index.php/Combining_rules
// for good reference.
// If a rule is being used it will overwrite any off diagonal parameters.
struct CombiningRule
{
  enum Value
  {
    NONE,
    LORENTZ,
    BERTHELOT,
    LORENTZ_BERTHELOT,
    UHRIN_PICKARD,
    UNKNOWN
  };
};

bool applyEnergyRule(::arma::mat & energyParams, const CombiningRule::Value rule);
bool applySizeRule(::arma::mat & sizeParams, const CombiningRule::Value rule);

CombiningRule::Value getRuleFromString(const ::std::string str);
::std::string getStringFromRule(const CombiningRule::Value rule);


}
}

#endif /* COMBINING_RULES_H */

