/*
 * PropertyChangeSupport.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef PROPERTY_CHANGE_SUPPORT_H
#define PROPERTY_CHANGE_SUPPORT_H

// INCLUDES ///////////////////////////////////
#include <string>
#include <vector>

// FORWARD DECLARES ///////////////////////////
namespace sstbx { namespace common { namespace event {
	class IPropertyChangeListener;
}}}

namespace sstbx { namespace common { namespace event {

class PropertyChangeSupport {
public:
	
	PropertyChangeSupport(void * source);

	void addPropertyChangeListener(IPropertyChangeListener * const listener);

	void removePropertyChangeListener(IPropertyChangeListener * const listener);

	void firePropertyChange(const std::string & propertyName, void * const oldValue, void * const newValue) const;

private:

	void * mySource;

	std::vector<IPropertyChangeListener *> listeners;
};

}}}

#endif /* PROPERTY_CHANGE_SUPPORT_H */
