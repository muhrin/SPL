/*
 * DataPool.h
 *
 *
 *  Created on: Jan 23, 2014
 *      Author: Martin Uhrin
 */

#ifndef DATA_POOL_DETAIL_H
#define DATA_POOL_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"

namespace spl {
namespace utility {

template< typename T>
  typename DataPool< T>::Ticket
  DataPool< T>::insert(const T & x)
  {
    Ticket ticket(generateTicketId(), this);
    myDataMap[ticket.getId()] = x;
    return ticket;
  }

template< typename T>
  T &
  DataPool< T>::get(const Ticket & ticket)
  {
    SSLIB_ASSERT(ticket.valid());
    const typename DataMap::iterator it = myDataMap.find(ticket.getId());

    SSLIB_ASSERT(it != myDataMap.end());
    return it->second;
  }

template< typename T>
  const T &
  DataPool< T>::get(const Ticket & ticket) const
  {
    SSLIB_ASSERT(ticket.valid());
    const typename DataMap::const_iterator it = myDataMap.find(ticket.getId());

    SSLIB_ASSERT(it != myDataMap.end());
    return it->second;
  }

template< typename T>
  typename DataPool< T>::iterator
  DataPool< T>::begin()
  {
    return iterator(myDataMap.begin());
  }

template< typename T>
  typename DataPool< T>::iterator
  DataPool< T>::end()
  {
    return iterator(myDataMap.end());
  }

template< typename T>
  typename DataPool< T>::const_iterator
  DataPool< T>::begin() const
  {
    return const_iterator(myDataMap.begin());
  }

template< typename T>
  typename DataPool< T>::const_iterator
  DataPool< T>::end() const
  {
    return const_iterator(myDataMap.end());
  }

template< typename T>
  void
  DataPool< T>::handleReleased(const size_t & id)
  {
    const typename DataMap::iterator it = myDataMap.find(id);
    SSLIB_ASSERT(it != myDataMap.end());
    myDataMap.erase(it);
  }

template< typename T>
  size_t
  DataPool< T>::generateTicketId()
  {
    if(myDataMap.empty())
      myLastTicketId = 0;
    else
    {
      if(myDataMap.rbegin()->first >= myDataMap.size())
      {
        // Must be a gap, find it
        size_t i = 0;
        BOOST_FOREACH(typename DataMap::const_reference x, myDataMap)
        {
          if(x.first != i)
          {
            myLastTicketId = i;
            break;
          }
          ++i;
        }
      }
      else
        myLastTicketId = myDataMap.rbegin()->first + 1;
    }
    return myLastTicketId;
  }

}
}

#endif /* DATA_POOL_DETAIL_H */
