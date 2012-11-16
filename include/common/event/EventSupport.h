/*
 * EventSupport.h
 *
 *
 *  Created on: Aug 31, 2012
 *      Author: Martin Uhrin
 */

#ifndef COMMON__EVENT_SUPPORT_H
#define COMMON__EVENT_SUPPORT_H

// INCLUDES /////////////////////////////////////////////
#include <algorithm>
#include <vector>

namespace pipelib {
namespace event {

template <class Listener>
class EventSupport
{
public:

  void insert(Listener & listener);
  bool remove(Listener & listener);

  template <class EventType>
  void notify(const EventType & evt);

private:

  typedef ::std::vector<Listener *> ListenersContainer;

  ListenersContainer    myListeners;
};

// IMPLEMENTATION ////////////////////////////////////////

template <class Listener>
void EventSupport<Listener>::insert(Listener & listener)
{
  myListeners.push_back(&listener);
}

template <class Listener>
bool EventSupport<Listener>::remove(Listener & listener)
{
  const typename ListenersContainer::iterator end = myListeners.end();
typename   ListenersContainer::iterator it = ::std::find(myListeners.begin(), end, &listener);

  if(it == end)
    return false;

  myListeners.erase(it);

  return true;
}

template <class Listener>
template <class EventType>
void EventSupport<Listener>::notify(const EventType &evt)
{
  const typename ListenersContainer::iterator end = myListeners.end();
  for(typename ListenersContainer::iterator it = myListeners.begin(); it != end; ++it)
  {
    (*it)->notify(evt);
  }
}


}
}

#endif /* EVENT_SUPPORT_H */
