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
    NONE, LORENTZ, BERTHELOT, UHRIN_PICKARD,

    NUM_RULES
  };
};

std::ostream &
operator <<(std::ostream & os, const CombiningRule::Value & rule);

std::istream &
operator >>(std::istream & is, CombiningRule::Value & rule);

double
applyRule(const CombiningRule::Value rule, const double x1, const double x2);

}
}

#endif /* COMBINING_RULES_H */

