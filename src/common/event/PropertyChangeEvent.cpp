/*
 * PropertyChangeEvent.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/event/PropertyChangeEvent.h"

namespace sstbx { namespace common { namespace event {

PropertyChangeEvent::PropertyChangeEvent(
	void * const source,
	const std::string & propertyName,
	void * const oldValue,
	void * const newValue):
mySource(source),
myPropertyName(propertyName),
myOldValue(oldValue),
myNewValue(newValue)
{}

}}}