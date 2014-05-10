/*
 * AnchorPointArrangement.cpp
 *
 *  Created on: Oct 17, 2013
 *      Author: Martin Uhrin
 */

#include "spl/analysis/AnchorArrangement.h"

#ifdef SPL_WITH_CGAL

#include "spl/SSLibAssert.h"

namespace spl {
namespace analysis {

AnchorPoint::AnchorPoint(const size_t idx, const ::arma::vec2 & pos,
    const double maxDisplacement) :
    idx_(idx), anchorPos_(pos), pos_(pos), maxDisplacement_(maxDisplacement)
{
}

const ::arma::vec2 &
AnchorPoint::getAnchorPos() const
{
  return anchorPos_;
}

const ::arma::vec2 &
AnchorPoint::getPos() const
{
  return pos_;
}

void
AnchorPoint::setPos(const ::arma::vec2 & newPos)
{
    pos_ = newPos;
}

double
AnchorPoint::getMaxDisplacement() const
{
  return maxDisplacement_;
}

size_t
AnchorPoint::idx() const
{
  return idx_;
}

}
}

#endif // SPL_WITH_CGAL
