/*
 * VectorAnchorArrangementOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef VECTOR_ANCHOR_ARRANGEMENT_OUTPUTTER_DETAIL_H
#define VECTOR_ANCHOR_ARRANGEMENT_OUTPUTTER_DETAIL_H

// INCLUDES ///////////////////
#include <iostream>

#include <armadillo>

#include <spl/analysis/AnchorArrangement.h>

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType>
  bool
  VectorAnchorArrangementOutputter< LabelType>::outputArrangement(
      const AnchorArrangement< LabelType> & arr) const
  {
    ::arma::vec2 source, dr;
    for(typename Arrangement::Arrangement::Edge_const_iterator edge =
        arr.getCgalArrangement().edges_begin(), edgesEnd =
        arr.getCgalArrangement().edges_end(); edge != edgesEnd; ++edge)
    {
      if(!edge->is_fictitious())
      {
        source = edge->source()->data().anchor->getPos();
        dr = edge->target()->data().anchor->getPos() - source;
        ::std::cout << source(0) << " " << source(1) << " " << dr(0) << " "
            << dr(1) << "\n";
      }
    }

    return true;
  }

template< typename LabelType>
  bool
  VectorAnchorArrangementOutputter< LabelType>::outputArrangement(
      const AnchorArrangement< LabelType> & arrangement,
      const InfoMap & labelInfo) const
  {
    return outputArrangement(arrangement);
  }

}
}

#endif /* VECTOR_ANCHOR_ARRANGEMENT_OUTPUTTER_DETAIL_H */
