/*
 * Iterator.h
 *
 *
 *  Created on: Nov 23, 2013
 *      Author: Martin Uhrin
 */

#ifndef ITERATOR_H
#define ITERATOR_H

// INCLUDES /////////////////////////////////////////////
#include <boost/iterator/transform_iterator.hpp>

#include "spl/utility/TransformFunctions.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template< typename Iterator>
  ::boost::transform_iterator< AddressOf< typename Iterator::value_type>,
      Iterator>
  makeAddressOfIterator(Iterator it)
  {
    return ::boost::transform_iterator<
        AddressOf< typename Iterator::value_type>, Iterator>(it,
            AddressOf< typename Iterator::value_type>());
  }

}
}

#endif /* ITERATOR_H */
