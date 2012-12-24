/*
 * SharedHandle.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SHARED_HANDLE_DETAIL_H
#define SHARED_HANDLE_DETAIL_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx {
namespace utility {

template <typename Id, class Notifiee>
SharedHandle<Id, Notifiee>::SharedHandle():
myNotifiee(NULL)
{}


template <typename Id, class Notifiee>
SharedHandle<Id, Notifiee>::SharedHandle(const Id id, Notifiee & notifiee):
myId(new Id(id)),
myNotifiee(&notifiee)
{}

template <typename Id, class Notifiee>
SharedHandle<Id, Notifiee>::SharedHandle(const SharedHandle & other):
myId(other.myId),
myNotifiee(other.myNotifiee)
{}

template <typename Id, class Notifiee>
SharedHandle<Id, Notifiee>::~SharedHandle()
{
  // Are we the last handle of this type
  if(myId.use_count() == 1)
  {
    Id id(*myId);
    myId.reset();
    myNotifiee->handleReleased(id);
  }
}

template <typename Id, class Notifiee>
SharedHandle<Id, Notifiee> &
SharedHandle<Id, Notifiee>::operator =(const SharedHandle & rhs)
{
  if(*this == rhs)
    return *this;

  // Are we the last handle of this type
  release();

  myId = rhs.myId;
  myNotifiee = rhs.myNotifiee;
  return *this;
}

template <typename Id, class Notifiee>
bool SharedHandle<Id, Notifiee>::operator ==(const SharedHandle & rhs) const
{
  if(!valid() && !rhs.valid()) // If both invalid then they are same
    return true;
  else if(!valid() || !rhs.valid()) // If only one is not valid they are different
    return false;

  // Do full compare
  return (*myId.get() == *rhs.myId.get()) && (myNotifiee == rhs.myNotifiee);
}

template <typename Id, class Notifiee>
const Id & SharedHandle<Id, Notifiee>::getId() const
{
  return *myId.get();
}

template <typename Id, class Notifiee>
bool SharedHandle<Id, Notifiee>::valid() const
{
  return myId.use_count() > 0;
}

template <typename Id, class Notifiee>
void SharedHandle<Id, Notifiee>::release()
{
  // Are we the last handle of this type
  if(myId.use_count() == 1)
  {
    Id id(*myId);
    myId.reset();
    myNotifiee->handleReleased(id);
  }
  myId.reset();
  myNotifiee = NULL;
}

}
}

#endif /* SHARED_HANDLE_DETAIL_H */
