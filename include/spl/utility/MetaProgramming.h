/*
 * MetaProgramming.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef META_PROGRAMMING_H
#define META_PROGRAMMING_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl { namespace utility {

template <bool Condition, typename TrueResult, typename FalseResult>
struct If_;
 
template <typename TrueResult, typename FalseResult>
struct If_<true, TrueResult, FalseResult>
{
  typedef TrueResult result;
};
 
template <typename TrueResult, typename FalseResult>
struct If_<false, TrueResult, FalseResult>
{
  typedef FalseResult result;
};


}}

#endif /* META_PROGRAMMING_H */
