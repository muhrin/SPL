/*
 * stablearison.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STABLE_COMPARISON_H
#define STABLE_COMPARISON_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {
namespace stable {

static const double STABLE_COMP_TOL = 1e-5;

bool lt(const double v1, const double v2, const double prec = STABLE_COMP_TOL);

bool gt(const double v1, const double v2, const double prec = STABLE_COMP_TOL);

bool eq(const double v1, const double v2, const double prec = STABLE_COMP_TOL);

bool neq(const double v1, const double v2, const double prec = STABLE_COMP_TOL);

bool leq(const double v1, const double v2, const double prec = STABLE_COMP_TOL);

bool geq(const double v1, const double v2, const double prec = STABLE_COMP_TOL);

double sign(const double v);

double round(const double v, const int dec);
}
}
}

#endif /* META_PROGRAMMING_H */
