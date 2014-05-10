/*
 * PointSeparator.h
 *
 *  Created on: Nov 28, 2013
 *      Author: Martin Uhrin
 */

#ifndef POINT_SEPARATOR_DETAIL_H
#define POINT_SEPARATOR_DETAIL_H

// INCLUDES ////////////
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

#include "spl/common/Structure.h"

namespace spl {
namespace build_cell {

template <typename Label>
void
SeparationData::setSeparationsFromLabels(const ::std::vector<Label> & pointLabels,
    ::std::map< utility::OrderedPair<Label>, double> & sepList)
{
  typedef utility::OrderedPair<Label> LabelPair;
  typedef ::std::map< LabelPair, double> Separations;

  const size_t numPoints = points.n_cols;
  separations.zeros();

  SSLIB_ASSERT(pointLabels.size() == numPoints);

  for(size_t i = 0 ; i < numPoints; ++i)
  {
    for(size_t j = i; j < numPoints; ++j)
    {
      const typename Separations::const_iterator it =
          sepList.find(LabelPair(pointLabels[i], pointLabels[j]));
      if(it != sepList.end())
        separations(i, j) = it->second;
    }
  }
}

}
}

#endif /* POINT_SEPARATOR_DETAIL_H */
