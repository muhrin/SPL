/*
 * IPropertyChangeListener.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_PROPERTY_CHANGE_LISTENER_H
#define I_PROPERTY_CHANGE_LISTENER_H

// INCLUDES ///////////////////////////////////

// FORWARD DECLARES ///////////////////////////
namespace sstbx { namespace common { namespace event {
	class PropertyChangeEvent;
}}}

namespace sstbx { namespace common { namespace event {

class IPropertyChangeListener {
public:
	virtual void propertyChange(const PropertyChangeEvent & evt) = 0;

};

}}}

#endif /* PROPERTY_CHANGE_LISTENER_H */
