/*
 * ParseLog.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_PARSE_H
#define SCHEMER_PARSE_H

// INCLUDES /////////////////////////////////////////////
#include <iostream>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {

struct ParseLogErrorCode
{
  enum Value
  {
    TYPE_CONVERSION_FAILED,
    REQUIRED_VALUE_MISSING,
    NODE_TYPE_WRONG,
    SEQUENCE_LENGTH_MISMATCH,
    UNRECOGNISED_ELEMENT
  };
};

class ParseLogError
{
public:
  ParseLogError(const ParseLogErrorCode::Value code,
      const std::string & path) :
      myCode(code), myPath(path)
  {
  }

  ParseLogError(const ParseLogErrorCode::Value code,
      const std::string & path, const std::string & message) :
      myCode(code), myPath(path), myMessage(message)
  {
  }

  ParseLogError &
  operator =(const ParseLogError & rhs);

  const ParseLogErrorCode::Value &
  getCode() const
  {
    return myCode;
  }

  const std::string &
  getPath() const
  {
    return myPath;
  }

  const std::string &
  getMessage() const
  {
    return myMessage;
  }

  void
  print(std::ostream * const os) const;

private:
  ParseLogErrorCode::Value myCode;
  std::string myPath;
  std::string myMessage;
};

class ParseLog
{
public:
  typedef std::vector< ParseLogError> ParseErrors;

  class PathPusher : boost::noncopyable
  {
  public:
    PathPusher(ParseLog * const log, const std::string & path);
    ~PathPusher();
  private:
    ParseLog * const myLog;
    const std::string & myPath;
  };

  bool
  hasErrors() const;
  const ParseErrors &
  getErrors() const;

  std::string
  pathString() const;

  void
  logError(const ParseLogErrorCode::Value code);
  void
  logError(const ParseLogErrorCode::Value code,
      const std::string & message);

  void
  printErrors(std::ostream * const os) const;
  void
  printErrors() const
  {
    printErrors(&std::cerr);
  }

private:
  typedef std::vector< std::string> ParsePath;

  void
  doPushPath(const std::string & path);
  void
  doPopPath();

  ParsePath myParsePath;
  ParseErrors myParseErrors;

  friend class PathPusher;
};

}

#endif /* SCHEMER_PARSE_H */
