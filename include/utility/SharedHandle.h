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


namespace sstbx {
namespace utility {

template <typename Id, class Notifiee>
class SharedHandle
{
  BOOST_CONCEPT_ASSERT((::boost::CopyConstructible<Id>));

public:
  SharedHandle(const Id id, Notifiee & notifiee);
  SharedHandle(const SharedHandle & other);
  ~SharedHandle();

  SharedHandle & operator =(SharedHandle & rhs);
  bool operator ==(const SharedHandle & rhs) const;

  const Id & getId() const;
  bool valid() const;
  void release();

private:
  typedef ::boost::shared_ptr<Id> IdPtr;

  IdPtr myId;
  Notifiee * myNotifiee;
};

}
}

#include "detail/SharedHandle.h"

#endif /* SHARED_HANDLE_H */
