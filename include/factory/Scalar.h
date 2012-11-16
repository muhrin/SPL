/*
 * Scalar.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCALAR_H
#define SCALAR_H

// INCLUDES /////////////////////////////////////////////

#include <boost/shared_ptr.hpp>

#include "factory/AbstractInputObject.h"
#include "factory/IInputObject.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class Scalar : public AbstractInputObject
{
public:

  typedef IInputObject::SharedPtrConstTyp InterfacePtrConst;

  Scalar(const ::std::string & value);

  // From IInputObject
  virtual size_t                  getSize() const;
  virtual const ::std::string &   getValue() const;
  // Access mechanism for sequences
  virtual const SharedPtrConstTyp  operator [](const size_t idx) const;
  // Access mechanism for maps
  virtual const SharedPtrConstTyp  operator [](const ::std::string & key) const;
  // End from IInputObject

private:

  const ::std::string     myValue;

};

}
}

#endif /* SCALAR_H */
