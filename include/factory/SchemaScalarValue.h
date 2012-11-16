/*
 * SchemaScalarValue.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_SCALAR_VALUE_H
#define SCHEMA_SCALAR_VALUE_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include "factory/ISchemaElementInstance.h"
#include "factory/SchemaElement.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class SchemaScalarValue : public SchemaElement, public ISchemaElementInstance
{
public:
  SchemaScalarValue(const ::std::string & name);
};

}
}

#endif /* SCHEMA_SCALAR_VALUE_H */
