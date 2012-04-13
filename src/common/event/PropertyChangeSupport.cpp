/*
 * PropertyChangeSupport.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/event/PropertyChangeSupport.h"

#include <algorithm>

#include "common/event/IPropertyChangeListener.h"
#include "common/event/PropertyChangeEvent.h"

namespace sstbx { namespace common { namespace event {

PropertyChangeSupport::PropertyChangeSupport(void * source):
mySource(source)
{
	
}

void PropertyChangeSupport::addPropertyChangeListener(IPropertyChangeListener * const listener)
{
	listeners.push_back(listener);
}

void PropertyChangeSupport::removePropertyChangeListener(IPropertyChangeListener *const listener)
{
	std::vector<IPropertyChangeListener *>::iterator it = std::find(listeners.begin(), listeners.end(), listener);
	if(it != listeners.end())
	{
		listeners.erase(it);
	}
}

void PropertyChangeSupport::firePropertyChange(const std::string & propertyName, void * const oldValue, void * const newValue) const
{
	using ::std::vector;

	PropertyChangeEvent evt(mySource, propertyName, oldValue, newValue);

	// Send this message to everyone that wants to know
	for(vector<IPropertyChangeListener *>::const_iterator it = listeners.begin(),
		end = listeners.end(); it != end; ++it)
	{
		(*it)->propertyChange(evt);
	}

}

}}}