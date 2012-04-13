/*
 * PropertyChangeEvent.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef PROPERTY_CHANGE_EVENT_H
#define PROPERTY_CHANGE_EVENT_H

// INCLUDES ///////////////////////////////////
#include <string>

// FORWARD DECLARES ///////////////////////////

namespace sstbx { namespace common { namespace event {

class PropertyChangeEvent {
public:
	PropertyChangeEvent(void * source, const std::string & propertyName, void * oldValue, void * newValue);

	void * getSource();

	const std::string & getPropertyName();

	void * getOldValue();

	void * getNewValue();

private:
	void *		mySource;
	std::string	myPropertyName;
	void *		myOldValue;
	void *		myNewValue;

};

}}}

#endif /* PROPERTY_CHANGE_EVENT_H */
