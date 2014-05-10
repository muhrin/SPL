/*
 * DataPool.h
 *
 *
 *  Created on: Jan 23, 2014
 *      Author: Martin Uhrin
 */

#ifndef DATA_POOL_H
#define DATA_POOL_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <map>

#include <boost/iterator/transform_iterator.hpp>

#include "spl/utility/SharedHandle.h"
#include "spl/utility/TransformFunctions.h"

namespace spl {
namespace utility {

template< typename T>
  class DataPool : SharedHandle< size_t>::Notifiee
  {
    typedef std::map< size_t, T> DataMap;

  public:
    typedef T value_type;
    typedef boost::transform_iterator<
        TakeSecond< typename DataMap::value_type>, typename DataMap::iterator> iterator;
    typedef boost::transform_iterator<
        TakeSecond< const typename DataMap::value_type>,
        typename DataMap::const_iterator> const_iterator;
    typedef SharedHandle< size_t> Ticket;

    virtual
    ~DataPool()
    {
    }

    Ticket
    insert(const T & x);

    T &
    get(const Ticket & ticket);
    const T &
    get(const Ticket & ticket) const;

    iterator
    begin();
    iterator
    end();

    const_iterator
    begin() const;
    const_iterator
    end() const;

  private:
    virtual void
    handleReleased(const size_t & id);
    size_t
    generateTicketId();

    size_t myLastTicketId;
    DataMap myDataMap;
  };

}
}

#include "spl/utility/detail/DataPool.h"

#endif /* DATA_POOL_H */
