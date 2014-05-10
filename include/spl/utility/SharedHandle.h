/*
 * SharedHandle.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SHARED_HANDLE_H
#define SHARED_HANDLE_H

// INCLUDES /////////////////////////////////////////////
#include <boost/concept_check.hpp>
#include <boost/shared_ptr.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template< typename Id>
  class SharedHandle
  {
    BOOST_CONCEPT_ASSERT((boost::CopyConstructible<Id>));
  public:
    typedef Id IdType;
    class Notifiee;

    SharedHandle();
    SharedHandle(const Id id, Notifiee * const notifiee = NULL);
    SharedHandle(const SharedHandle & other);
    ~SharedHandle();

    SharedHandle & operator =(const SharedHandle & rhs);
    bool operator ==(const SharedHandle & rhs) const;

    const Id & getId() const;
    bool valid() const;
    void release();

  private:
    typedef boost::shared_ptr<Id> IdPtr;

    IdPtr myId;
    Notifiee * myNotifiee;
  };

template< typename T>
  class SharedHandle< T>::Notifiee
  {
  public:
    virtual
    ~Notifiee()
    {
    }
    virtual void
    handleReleased(const T & id) = 0;
  };

}
}

#include "detail/SharedHandle.h"

#endif /* SHARED_HANDLE_H */
