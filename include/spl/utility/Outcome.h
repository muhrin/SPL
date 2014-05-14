/*
 * Outcome.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef OUTCOME_H
#define OUTCOME_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <string>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

class Outcome
{
public:
  static Outcome
  success();
  static Outcome
  failure(const std::string & message = "");

  Outcome();

  bool
  isSuccess() const;
  operator bool() const;

  void
  setSuccess();
  void
  setFailure(const std::string & message = "");
  const std::string &
  getMessage() const;

private:
  bool mySuccess;
  std::string myMsg;
};

template< typename ErrorCode>
  class OutcomeWithErrorCode : public Outcome
  {
  public:
    static OutcomeWithErrorCode
    success();
    static OutcomeWithErrorCode
    failure(const std::string & message = "");
    static OutcomeWithErrorCode
    failure(const ErrorCode & errorCode, const std::string & message = "");

    OutcomeWithErrorCode();

    void
    setErrorCode(const ErrorCode & code);
    const ErrorCode &
    getErrorCode() const;

  private:
    ErrorCode myErrorCode;
  };

template< typename ErrorCode>
  OutcomeWithErrorCode< ErrorCode>
  OutcomeWithErrorCode< ErrorCode>::success()
  {
    OutcomeWithErrorCode outcome;
    outcome.setSuccess();
    return outcome;
  }

template< typename ErrorCode>
  OutcomeWithErrorCode< ErrorCode>
  OutcomeWithErrorCode< ErrorCode>::failure(const std::string & message)
  {
    OutcomeWithErrorCode outcome;
    outcome.setFailure(message);
    return outcome;
  }

template< typename ErrorCode>
  OutcomeWithErrorCode< ErrorCode>
  OutcomeWithErrorCode< ErrorCode>::failure(const ErrorCode & errorCode,
      const std::string & message)
  {
    OutcomeWithErrorCode outcome;
    outcome.setFailure(message);
    outcome.setErrorCode(errorCode);
    return outcome;
  }

template< typename ErrorCode>
  OutcomeWithErrorCode< ErrorCode>::OutcomeWithErrorCode() :
      Outcome()
  {
  }

template< typename ErrorCode>
  void
  OutcomeWithErrorCode< ErrorCode>::setErrorCode(const ErrorCode & code)
  {
    myErrorCode = code;
  }

template< typename ErrorCode>
  const ErrorCode &
  OutcomeWithErrorCode< ErrorCode>::getErrorCode() const
  {
    return myErrorCode;
  }

}
}

#endif /* OUTCOME_H */
