/*
 * AtomFormatParser.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef ATOM_FORMAT_PARSER_DETAIL_H
#define ATOM_FORMAT_PARSER_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <armadillo>

#include "spl/factory/SsLibYamlSchema.h"
#include "spl/utility/Armadillo.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace io {

template< typename AtomDataType>
  AtomFormatParser< AtomDataType>::AtomFormatParser()
  {
    init< AtomDataType>();
  }

template< typename AtomDataType>
  AtomFormatParser< AtomDataType>::AtomFormatParser(
      const FormatDescription & formatDescription)
  {
    init< AtomDataType>();
    setFormat(formatDescription);
  }

template< typename AtomDataType>
  AtomFormatParser< AtomDataType> &
  AtomFormatParser< AtomDataType>::operator =(const AtomFormatParser & toCopy)
  {
    myDefaults = toCopy.myDefaults;
    myFormatOrder = toCopy.myFormatOrder;
    myFormatEntries = toCopy.myFormatEntries;
    myFormatEntriesList = toCopy.myFormatEntriesList;
    return *this;
  }

template< typename AtomDataType>
  bool
  AtomFormatParser< AtomDataType>::setFormat(
      const FormatDescription & formatDescription)
  {
    FormatOrder newFormatOrder;

    bool succeeded = true;
    BOOST_FOREACH(const std::string & formatString, formatDescription)
    {
      std::vector< std::string> strs;
      boost::split(strs, formatString, boost::is_any_of("="));
      if(strs.size() > 2)
      {
        succeeded = false;
        break;
      }
      boost::algorithm::trim(strs[0]);

      const typename FormatEntries::const_iterator it = myFormatEntries.find(
          strs[0]);
      if(it == myFormatEntries.end())
      {
        succeeded = false;
        break;
      }

      newFormatOrder.push_back(it->second);

      // Check if there is a default value
      if(strs.size() == 2)
      {
        // TODO: Complete this
      }
    }

    if(succeeded)
      myFormatOrder = newFormatOrder;

    return succeeded;
  }

template< typename AtomDataType>
  template< typename T>
    void
    AtomFormatParser< AtomDataType>::addEntry(const std::string & name,
        T AtomDataType::* const memberPtr)
    {
      const CanonicalMemberPtrType canonical = canonicalise(memberPtr);
      myFormatEntries[name] = canonical;
      myFormatEntriesList.push_back(canonical);
    }

template< typename AtomDataType>
  template< typename T>
    bool
    AtomFormatParser< AtomDataType>::setDefault(
        T AtomDataType::* const memberPtr, const boost::optional< T> & value)
    {
      const size_t idx = entryIdx(memberPtr);
      if(idx == myFormatEntriesList.size())
        return false;

      if(value)
        myDefaults[idx] = *value;
      else
        myDefaults[idx] = boost::any();
      return true;
    }

template< typename AtomDataType>
  template< typename T>
    bool
    AtomFormatParser< AtomDataType>::setDefault(
        boost::optional< T> AtomDataType::* const memberPtr,
        const boost::optional< T> & value)
    {
      const size_t idx = entryIdx(memberPtr);
      if(idx == myFormatEntriesList.size())
        return false;

      if(myDefaults.size() < myFormatEntries.size())
        myDefaults.resize(myFormatEntries.size());

      if(value)
        myDefaults[idx] = *value;
      else
        myDefaults[idx] = boost::any();
      return true;
    }

template< typename AtomDataType>
  template< typename T>
    boost::optional< T>
    AtomFormatParser< AtomDataType>::getValue(T AtomDataType::* const memberPtr,
        const AtomInfo & atomData) const
    {
      boost::optional< T> value;
      {
        const AtomDataType * const dataClass = boost::get< AtomDataType>(
            &atomData);
        if(dataClass)
          value = dataClass->*memberPtr;
        else
        {
          const CompactAtomInfo * const vector = boost::get< CompactAtomInfo>(
              &atomData);
          const size_t idx = formatIdx(memberPtr);
          if(vector && idx != myFormatOrder.size() && idx < vector->size())
            value = boost::lexical_cast< T>((*vector)[idx]);
        }
      }

      // Try to get a default
      if(!value)
      {
        const size_t idx = entryIdx(memberPtr);
        if(idx != myFormatEntries.size() && !myDefaults[idx].empty())
          value = boost::any_cast< T>(myDefaults[idx]);
      }

      return value;
    }

template< typename AtomDataType>
  template< typename T>
    boost::optional< T>
    AtomFormatParser< AtomDataType>::getValue(
        boost::optional< T> AtomDataType::* const memberPtr,
        const AtomInfo & atomData) const
    {
      boost::optional< T> value;
      {
        const AtomDataType * const dataClass = boost::get< AtomDataType>(
            &atomData);
        if(dataClass)
          value = dataClass->*memberPtr;
        else
        {
          const CompactAtomInfo * const vector = boost::get< CompactAtomInfo>(
              &atomData);
          const size_t idx = formatIdx(memberPtr);
          if(vector && idx != myFormatOrder.size() && idx < vector->size())
            value = boost::lexical_cast< T>((*vector)[idx]);
        }
      }

      // Try to get a default
      if(!value)
      {
        const size_t idx = entryIdx(memberPtr);
        if(idx != myFormatEntries.size() && !myDefaults[idx].empty())
          value = boost::any_cast< T>(myDefaults[idx]);
      }

      return value;
    }

template< typename AtomDataType>
  template< typename T>
    typename AtomFormatParser< AtomDataType>::CanonicalMemberPtrType
    AtomFormatParser< AtomDataType>::canonicalise(
        T AtomDataType::* const memberPtr) const
    {
      return reinterpret_cast< CanonicalMemberPtrType>(memberPtr);
    }

template< typename AtomDataType>
  template< typename T>
    size_t
    AtomFormatParser< AtomDataType>::entryIdx(
        T AtomDataType::* const memberPtr) const
    {
      const typename FormatEntriesList::const_iterator it = std::find(
          myFormatEntriesList.begin(), myFormatEntriesList.end(),
          canonicalise(memberPtr));

      return it - myFormatEntriesList.begin();
    }

template< typename AtomDataType>
  template< typename T>
    size_t
    AtomFormatParser< AtomDataType>::formatIdx(
        T AtomDataType::* const memberPtr) const
    {
      const typename FormatOrder::const_iterator it = std::find(
          myFormatOrder.begin(), myFormatOrder.end(), canonicalise(memberPtr));

      return it - myFormatOrder.begin();
    }

} // namespace io
} // namespace spl

#endif /* ATOM_FORMAT_PARSER_DETAIL_H */
