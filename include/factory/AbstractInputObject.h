/*
 * AbstractInputObject.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_INPUT_OBJECT_H
#define ABSTRACT_INPUT_OBJECT_H

// INCLUDES /////////////////////////////////////////////
#include <string>
#include <memory>

#include <boost/variant.hpp>

#include "factory/IInputObject.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class AbstractInputObject : public IInputObject
{
public:

  static const char * EMPTY_VALUE;

  virtual Type                    getType() const = 0;

protected:

  AbstractInputObject(const Type type);

private:

  const Type                     myType;

};

}
}

#endif /* ABSTRACT_INPUT_OBJECT_H */
