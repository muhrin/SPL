/*
 * GenericEventSupport.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef GENERIC_EVENT_SUPPORT_H
#define GENERIC_EVENT_SUPPORT_H

// INCLUDES ///////////////////////////////////
#include "IGenericEventListener.h"

#include <vector>
#include <algorithm>

// FORWARD DECLARES ///////////////////////////

namespace sstbx { namespace common { namespace event {

template <class EventType>
class GenericEventSupport
{
public:

	typedef typename ::std::vector<IGenericEventListener<EventType> *> ListenersVec;

	void addListener(IGenericEventListener<EventType> * const listener)
	{
		listeners.push_back(listener);
	}

	void removeListener(IGenericEventListener<EventType> * const listener)
	{
		typename ::std::vector<IGenericEventListener<EventType> *>::iterator it = 
			::std::find(listeners.begin(), listeners.end(), listener);

		if(it != listeners.end())
			listeners.erase(it);
	}


	void fireEvent(const EventType & evt)
	{
		for(typename ListenersVec::const_iterator it = listeners.begin(), end = listeners.end();
			it != end; ++it)
		{
			(*it)->eventFired(evt);
		}
	}

private:
	ListenersVec listeners;
};

}}}

#endif /* GENERIC_EVENT_SUPPORT_H */
