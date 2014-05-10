/*
 * ParseLog.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "schemer/Parse.h"

#include <iostream>
#include <sstream>

#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>

#include "schemer/Assert.h"

// NAMESPACES ////////////////////////////////

namespace schemer {

ParseLogError &
ParseLogError::operator =(const ParseLogError & rhs)
{
  myCode = rhs.myCode;
  myPath = rhs.myPath;
  myMessage = rhs.myMessage;
  return *this;
}

void
ParseLogError::print(std::ostream * const os) const
{
  SCHEMER_ASSERT(os);

  *os << myPath << ": " << myMessage << " code: " << myCode
      << std::endl;
}

ParseLog::PathPusher::PathPusher(ParseLog * const log,
    const std::string & path) :
    myLog(log), myPath(path)
{
  if(myLog)
    myLog->doPushPath(myPath);
}

ParseLog::PathPusher::~PathPusher()
{
  if(myLog)
    myLog->doPopPath();
}

bool
ParseLog::hasErrors() const
{
  return !myParseErrors.empty();
}

const ParseLog::ParseErrors &
ParseLog::getErrors() const
{
  return myParseErrors;
}

::std::string
ParseLog::pathString() const
{
  return boost::algorithm::join(myParsePath, ".");
}

void
ParseLog::logError(const ParseLogErrorCode::Value code)
{
  myParseErrors.push_back(ParseLogError(code, pathString()));
}

void
ParseLog::logError(const ParseLogErrorCode::Value code,
    const std::string & message)
{
  myParseErrors.push_back(ParseLogError(code, pathString(), message));
}

void
ParseLog::printErrors(std::ostream * const os) const
{
  BOOST_FOREACH(const ParseLogError & error, myParseErrors)
    error.print(os);
}

void
ParseLog::doPushPath(const std::string & path)
{
  myParsePath.push_back(path);
}

void
ParseLog::doPopPath()
{
  myParsePath.pop_back();
}

}

