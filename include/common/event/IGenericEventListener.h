/*
 * IGenericEventListener.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_GENERIC_EVENT_LISTENER_H
#define I_GENERIC_EVENT_LISTENER_H

// INCLUDES ///////////////////////////////////
#include <vector>

// FORWARD DECLARES ///////////////////////////

namespace sstbx { namespace common { namespace event {

template <class EventType>
class IGenericEventListener
{
public:
	virtual void eventFired(const EventType & evt) = 0;
};

}}}

#endif /* GENERIC_EVENT_LISTENER_H */
