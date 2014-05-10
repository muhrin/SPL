/*
 * List.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_LIST_DETAIL_H
#define SCHEMER_LIST_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <sstream>

#include <boost/foreach.hpp>

#include "schemer/Registry.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {

template< class T>
  List< T>::List() :
      myLength(-1), myEntryType()
  {
  }

template< class T>
  List< T>::List(const List & toCopy) :
      myLength(toCopy.myLength), myEntryType(toCopy.myEntryType)
  {
  }

template< class T>
  bool
  List< T>::valueToNode(const BindingType & list, YAML::Node * const node) const
  {
    if(myLength != -1 && myLength != list.size())
      return false;

    BOOST_FOREACH(typename BindingType::const_reference value, list)
    {
      YAML::Node listEntry;
      if(myEntryType.valueToNode(value, &listEntry))
        node->push_back(listEntry);
    }
    return true;
  }

template< class T>
  bool
  List< T>::nodeToValue(const YAML::Node & node, BindingType * const list,
      ParseLog * const log) const
  {
    if(!node.IsSequence())
    {
      if(log)
        log->logError(ParseLogErrorCode::NODE_TYPE_WRONG,
            "Expected sequence.");
      return false;
    }

    if(myLength != -1 && myLength != static_cast< int>(node.size()))
    {
      if(log)
        log->logError(ParseLogErrorCode::SEQUENCE_LENGTH_MISMATCH,
            "Sequence is wrong length");
      return false;
    }

    for(size_t i = 0; i < node.size(); ++i)
    {
      ::std::stringstream ss;
      ss << "[" << i << "]";
      ParseLog::PathPusher pusher(log, ss.str());

      typename T::BindingType entryValue;
      if(myEntryType.nodeToValue(node[i], &entryValue, log))
        list->push_back(entryValue);
    }

    return true;
  }

template< class EntrySchema>
  List< EntrySchema> *
  List< EntrySchema>::length(const int length)
  {
    myLength = length;
    return this;
  }

template< class T>
  List< T> *
  List< T>::clone() const
  {
    return new List< T>(*this);
  }

}

#endif /* SCHEMER_LIST_DETAIL_H */
