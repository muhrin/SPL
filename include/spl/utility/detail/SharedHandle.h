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


namespace spl {
namespace utility {

template <typename Id>
SharedHandle<Id>::SharedHandle():
myNotifiee(NULL)
{}


template <typename Id>
SharedHandle<Id>::SharedHandle(const Id id, Notifiee * notifiee):
myId(new Id(id)),
myNotifiee(notifiee)
{}

template <typename Id>
SharedHandle<Id>::SharedHandle(const SharedHandle & other):
myId(other.myId),
myNotifiee(other.myNotifiee)
{}

template <typename Id>
SharedHandle<Id>::~SharedHandle()
{
  release();
}

template <typename Id>
SharedHandle<Id> &
SharedHandle<Id>::operator =(const SharedHandle & rhs)
{
  if(*this == rhs)
    return *this;

  // Are we the last handle of this type
  release();

  myId = rhs.myId;
  myNotifiee = rhs.myNotifiee;
  return *this;
}

template <typename Id>
bool SharedHandle<Id>::operator ==(const SharedHandle & rhs) const
{
  if(!valid() && !rhs.valid()) // If both invalid then they are same
    return true;
  else if(!valid() || !rhs.valid()) // If only one is not valid they are different
    return false;

  // Do full compare
  return (*myId.get() == *rhs.myId.get()) && (myNotifiee == rhs.myNotifiee);
}

template <typename Id>
const Id & SharedHandle<Id>::getId() const
{
  return *myId.get();
}

template <typename Id>
bool SharedHandle<Id>::valid() const
{
  return myId.use_count() > 0;
}

template <typename Id>
void SharedHandle<Id>::release()
{
  // Are we the last handle of this type
  if(myId.use_count() == 1)
  {
    Id id(*myId);
    myId.reset();
    if(myNotifiee)
      myNotifiee->handleReleased(id);
  }
  myId.reset();
  myNotifiee = NULL;
}

}
}

#endif /* SHARED_HANDLE_DETAIL_H */
