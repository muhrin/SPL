/*
 * IParameterisable.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_PARAMETERISABLE_H
#define I_PARAMETERISABLE_H

// INCLUDES /////////////////////////////////////////////
#include <utility>
#include <string>
#include <vector>

// DEFINES //////////////////////////////////////////////

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////

namespace potential {

class IParameterisable
{
public:
  virtual
  ~IParameterisable()
  {
  }

  // Get the current parameters vector for this potential.
  virtual std::vector< std::string>
  getParams() const = 0;

  // Set the parameters for this potential
  virtual bool
  setParams(const std::vector< std::string> & params)
  {
    return setParams(params, NULL);
  }
  virtual bool
  setParams(const std::vector< std::string> & params, std::string * const errorMsg) = 0;
};

}
}

#endif /* I_PARAMETERISABLE_H */
