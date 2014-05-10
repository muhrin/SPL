/*
 * Outcome.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/utility/Outcome.h"

namespace spl {
namespace utility {

Outcome Outcome::success()
{
  Outcome outcome;
  outcome.setSuccess();
  return outcome;
}

Outcome Outcome::failure(const ::std::string & msg)
{
  Outcome outcome;
  outcome.setFailure(msg);
  return outcome;
}

Outcome::Outcome():
mySuccess(false)
{}

bool Outcome::isSuccess() const
{
  return mySuccess;
}

Outcome::operator bool() const
{
  return mySuccess;
}

void Outcome::setSuccess()
{
  mySuccess = true;
}

void Outcome::setFailure(const ::std::string & message)
{
  myMsg = message;
}

const ::std::string & Outcome::getMessage() const
{
  return myMsg;
}


}
}
