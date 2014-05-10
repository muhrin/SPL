/*
 * Map.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_MAP_DETAIL_H
#define SCHEMER_MAP_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/scoped_ptr.hpp>

#include "schemer/Registry.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {
namespace detail {

template< typename T>
  class HomoMapElement
  {
    typedef typename T::BindingType BindingType;
  public:
    HomoMapElement() :
        myRequired(false)
    {
    }

    HomoMapElement *
    required()
    {
      myRequired = true;
      return this;
    }

    bool
    isRequired() const
    {
      return myRequired;
    }

    HomoMapElement *
    defaultValue(const BindingType & defaultValue)
    {
      myDefault = defaultValue;
      return this;
    }
    const boost::optional< BindingType>
    getDefault() const
    {
      return myDefault;
    }

    bool
    valueToNode(const BindingType & value, YAML::Node * const node) const
    {
      return myType.valueToNode(value, node);
    }
    bool
    nodeToValue(const YAML::Node & node, BindingType * const value,
        ParseLog * const log) const
    {
      if(!node.IsDefined() || node.IsNull())
      {
        // Check for a default
        if(myDefault)
        {
          *value = *myDefault;
          return true;
        }

        if(log)
          log->logError(ParseLogErrorCode::REQUIRED_VALUE_MISSING,
              "Required map element missing.");

        return false;
      }

      return myType.nodeToValue(node, value, log);
    }
    bool
    defaultValueToNode(YAML::Node & node) const
    {
      if(!myDefault)
        return false;

      return valueToNode(*myDefault, node);
    }

  private:
    const T myType;
    boost::optional< BindingType> myDefault;
    bool myRequired;
  };

template< class T>
  class HeteroMapElementBase
  {
  public:
    virtual
    ~HeteroMapElementBase()
    {
    }

    virtual bool
    isRequired() const = 0;
    virtual bool
    hasDefault() const = 0;

    virtual bool
    valueToNode(const T & map, YAML::Node * const node) const = 0;
    virtual bool
    nodeToValue(const YAML::Node & node, T * const map,
        ParseLog * const log) const = 0;
    virtual bool
    defaultValueToMap(T * const map) const = 0;

    virtual HeteroMapElementBase *
    clone() const = 0;
  };

template< class T>
  HeteroMapElementBase< T> *
  new_clone(const HeteroMapElementBase< T> & entry)
  {
    return entry.clone();
  }

template< class MapBindingType, typename MapBindingMemberType, typename T>
  class HeteroMapElement : public HeteroMapElementBase< MapBindingType>
  {
    typedef T BindingType;
    typedef MapBindingMemberType (MapBindingType::* const Member);
    typedef detail::Type< T> ElementType;

  public:
    HeteroMapElement(const ElementType & elementType, Member member) :
        myType(elementType), myMember(member)
    {
    }
    virtual
    ~HeteroMapElement()
    {
    }

    HeteroMapElement *
    defaultValue(const BindingType & defaultValue)
    {
      myDefault = defaultValue;
      return this;
    }

    // From HeteroMapElementBase ///////////////
    virtual bool
    isRequired() const
    {
      return true;
    }

    virtual bool
    hasDefault() const
    {
      return myDefault;
    }

    virtual bool
    valueToNode(const MapBindingType & map, YAML::Node * const node) const
    {
      const MapBindingMemberType value = map.*myMember;
      return myType.valueToNode(value, node);
    }

    virtual bool
    nodeToValue(const YAML::Node & node, MapBindingType * const map,
        ParseLog * const log) const
    {
      if(!node.IsDefined() || node.IsNull())
      {
        // Check for a default
        if(defaultValueToMap(map))
          return true;

        if(log)
          log->logError(ParseLogErrorCode::REQUIRED_VALUE_MISSING,
              "Required map entry missing.");

        return false;
      }

      T value;
      if(!myType.nodeToValue(node, &value, log))
        return false;

      *map.*myMember = value;
      return true;
    }

    virtual bool
    defaultValueToMap(MapBindingType * const map) const
    {
      if(!myDefault)
        return false;

      *map.*myMember = *myDefault;
      return true;
    }

    virtual HeteroMapElement *
    clone() const
    {
      return new HeteroMapElement(*this);
    }
    // End from HeteroMapElementBase /////////////

  private:
    const ElementType & myType;
    Member myMember;
    boost::optional< MapBindingMemberType> myDefault;
  };

template< class MapBindingType, typename MapBindingMemberType, typename T>
  class HeteroMapElement< MapBindingType,
      boost::optional< MapBindingMemberType>, T> : public HeteroMapElementBase<
      MapBindingType>
  {
    typedef typename boost::optional< MapBindingMemberType> BindingType;
    typedef BindingType (MapBindingType::* const Member);
    typedef detail::Type< T> ElementType;

  public:
    HeteroMapElement(const ElementType & elementType, Member member) :
        myType(elementType), myMember(member)
    {
    }
    virtual
    ~HeteroMapElement()
    {
    }

    HeteroMapElement *
    defaultValue(const MapBindingMemberType & defaultValue)
    {
      myDefault = defaultValue;
      return this;
    }

    // From HeteroMapElementBase ///////////////
    virtual bool
    isRequired() const
    {
      return false;
    }

    virtual bool
    hasDefault() const
    {
      return myDefault;
    }

    virtual bool
    valueToNode(const MapBindingType & map, YAML::Node * const node) const
    {
      if(!(map.*myMember))
        return false;

      const MapBindingMemberType value = *(map.*myMember);
      return myType.valueToNode(value, node);
    }
    virtual bool
    nodeToValue(const YAML::Node & node, MapBindingType * const map,
        ParseLog * const log) const
    {
      if(!node.IsDefined() || node.IsNull())
      {
        // Check for a default
        if(!defaultValueToMap(map) && node.IsDefined())
          map->*myMember = MapBindingMemberType();
        return true;
      }

      T value;
      if(!myType.nodeToValue(node, &value, log))
        return false;

      *map.*myMember = value;
      return true;
    }
    virtual bool
    defaultValueToMap(MapBindingType * const map) const
    {
      map->*myMember = myDefault;
      return myDefault;
    }
    virtual HeteroMapElement *
    clone() const
    {
      return new HeteroMapElement(*this);
    }
    // End from HeteroMapElementBase /////////////

  private:
    const ElementType & myType;
    Member myMember;
    boost::optional< MapBindingMemberType> myDefault;
  };

template< class DerivedBinding, typename BaseBinding>
  class HeteroMapBaseEntryWrapper : public HeteroMapElementBase< DerivedBinding>
  {
  public:
    explicit
    HeteroMapBaseEntryWrapper(
        const HeteroMapElementBase< BaseBinding> & element) :
        myEntry(element.clone())
    {
    }
    HeteroMapBaseEntryWrapper(const HeteroMapBaseEntryWrapper & toCopy) :
        myEntry(toCopy.myEntry->clone())
    {
    }
    virtual
    ~HeteroMapBaseEntryWrapper()
    {
    }

    virtual bool
    isRequired() const
    {
      return myEntry->isRequired();
    }
    virtual bool
    hasDefault() const
    {
      return myEntry->hasDefault();
    }
    virtual bool
    valueToNode(const DerivedBinding & map, YAML::Node * const node) const
    {
      return myEntry->valueToNode(map, node);
    }
    virtual bool
    nodeToValue(const YAML::Node & node, DerivedBinding * const map,
        ParseLog * const log) const
    {
      return myEntry->nodeToValue(node, map, log);
    }
    virtual bool
    defaultValueToMap(DerivedBinding * const map) const
    {
      return myEntry->defaultValueToMap(map);
    }
    virtual HeteroMapBaseEntryWrapper *
    clone() const
    {
      return new HeteroMapBaseEntryWrapper(*this);
    }

  private:
    boost::scoped_ptr< HeteroMapElementBase< BaseBinding> > myEntry;
  };

} // namespace detail

template< typename KeyType, typename EntryType>
  Map< KeyType, EntryType>::Map() :
      myKey(getTypeInstance< KeyType>()), myEntry(
          getTypeInstance< EntryType>()), myAllowUnknownEntries(true)
  {
  }

template< typename KeyType, typename EntryType>
  bool
  Map< KeyType, EntryType>::valueToNode(const BindingType & value,
      YAML::Node * const node) const
  {
    typename EntriesMap::const_iterator it;
    BOOST_FOREACH(typename BindingType::const_reference entry, value)
    {
      it = myEntries.find(entry.first);
      if(it != myEntries.end())
      {
        YAML::Node entryNode;
        it->second.valueToNode(entry.second, &entryNode);
        (*node)[entry.first] = entryNode;
      }
      else if(myAllowUnknownEntries)
      {
        YAML::Node entryNode;
        myEntry.valueToNode(entry.second, &entryNode);
        (*node)[entry.first] = entryNode;
      }
    }
    return true;
  }

template< typename KeyType, typename EntryType>
  bool
  Map< KeyType, EntryType>::nodeToValue(const YAML::Node & node,
      BindingType * const value, ParseLog * const log) const
  {
    if(!node.IsMap())
    {
      if(log)
        log->logError(ParseLogErrorCode::NODE_TYPE_WRONG, "Expected map");
      return false;
    }

    typename EntriesMap::const_iterator entriesIt;
    for(YAML::Node::const_iterator it = node.begin(), end = node.end();
        it != end; ++it)
    {
      typename BindingType::key_type key;
      if(!myKey.nodeToValue(it->first, &key, log))
      {
        log->logError(ParseLogErrorCode::UNRECOGNISED_ELEMENT,
            "Invalid key type: " + it->first.Scalar());
        continue;
      }

      ParseLog::PathPusher pusher(log, it->first.Scalar());

      entriesIt = myEntries.find(key);
      if(entriesIt != myEntries.end())
      {
        typename BindingType::mapped_type mappedValue;
        if(entriesIt->second.nodeToValue(it->second, &mappedValue, log))
          (*value)[key] = mappedValue;
      }
      else if(myAllowUnknownEntries)
      {
        typename BindingType::mapped_type mappedValue;
        if(myEntry.nodeToValue(it->second, &mappedValue, log))
          (*value)[key] = mappedValue;
      }
    }
    return true;
  }

template< typename KeyType, typename EntryType>
  typename Map< KeyType, EntryType>::Element *
  Map< KeyType, EntryType>::element(const std::string & name)
  {
    return &myEntries[name];
  }

template< typename KeyType, typename EntryType>
  Map< KeyType, EntryType> *
  Map< KeyType, EntryType>::clone() const
  {
    return new Map< KeyType, EntryType>(*this);
  }

template< typename KeyType, typename EntryType>
  bool
  Map< KeyType, EntryType>::areUnknownEntriesAllowed() const
  {
    return myAllowUnknownEntries;
  }

template< typename KeyType, typename EntryType>
  void
  Map< KeyType, EntryType>::setAllowUnknownEntries(
      const bool allowUnknownEntries)
  {
    myAllowUnknownEntries = allowUnknownEntries;
  }

template< class T>
  typename HeteroMap< T>::EntryIterator
  HeteroMap< T>::entriesBegin() const
  {
    return myEntries.begin();
  }

template< class T>
  typename HeteroMap< T>::EntryIterator
  HeteroMap< T>::entriesEnd() const
  {
    return myEntries.end();
  }

template< class BindingType>
  bool
  HeteroMap< BindingType>::valueToNode(const BindingType & value,
      YAML::Node * const node) const
  {
    BOOST_FOREACH(typename EntriesMap::const_reference entry, myEntries)
    {
      YAML::Node entryNode;
      if(entry.second->valueToNode(value, &entryNode))
        (*node)[entry.first] = entryNode;
    }
    return true;
  }

template< typename BindingType>
  bool
  HeteroMap< BindingType>::nodeToValue(const YAML::Node & node,
      BindingType * const map, ParseLog * const log) const
  {
    if(node.IsNull())
    {
      // We have a map with no entries, check that we have no required entries without
      // a default
      bool succeeded = true;
      const YAML::Node nullNode;
      BOOST_FOREACH(typename EntriesMap::const_reference entry, myEntries)
      {
        if(!entry.second->nodeToValue(nullNode, map, log))
          succeeded = false;
      }
      return succeeded;
    }

    if(!node.IsMap())
    {
      if(log)
        log->logError(ParseLogErrorCode::NODE_TYPE_WRONG, "Expected map.");
      return false;
    }

    // First populate all the elements we need to process so we can check them off
    std::set< std::string> toProcess;
    BOOST_FOREACH(typename EntriesMap::const_reference entry, myEntries)
    {
      toProcess.insert(entry.first);
    }

    // Now go through the node checking each complete element off
    std::set< std::string>::const_iterator process;
    for(YAML::Node::const_iterator it = node.begin(), end = node.end();
        it != end; ++it)
    {
      const std::string & entryName = it->first.Scalar();
      process = toProcess.find(entryName);
      if(process != toProcess.end())
      {
        ParseLog::PathPusher pusher(log, entryName);
        myEntries.find(entryName)->second->nodeToValue(it->second, map, log);
        toProcess.erase(process);
      }
      else
      {
        if(log)
          log->logError(ParseLogErrorCode::UNRECOGNISED_ELEMENT,
              "Unrecognised map element: " + entryName);
      }
    }

    // Finally see if there are any left that need processing
    BOOST_FOREACH(const std::string & e, toProcess)
    {
      const Entry & entry = *myEntries.find(e)->second;
      if(entry.isRequired())
      {
        if(entry.hasDefault())
          entry.defaultValueToMap(map);
        else
        {
          if(log)
            log->logError(ParseLogErrorCode::REQUIRED_VALUE_MISSING,
                "Required element missing: " + e);
        }
      }
    }

    return true;
  }

template< class T>
  HeteroMap< T> *
  HeteroMap< T>::clone() const
  {
    return new HeteroMap(*this);
  }

template< class BindingType>
  template< typename ElementType, typename MemberType>
    detail::HeteroMapElement< BindingType, MemberType,
        typename ElementType::BindingType> *
    HeteroMap< BindingType>::element(const std::string & name,
        MemberType (BindingType::* const member))
    {
      detail::HeteroMapElement< BindingType, MemberType,
          typename ElementType::BindingType> * const element =
          new detail::HeteroMapElement< BindingType, MemberType,
              typename ElementType::BindingType>(
              getTypeInstance< ElementType>(), member);

      // WARNING: This will only insert the new entry if one with the same name doesn't
      // exist already.  Otherwise the new one will be destroyed and the old one returned.
      // This may not be what the user expects.
      myEntries.insert(name, element);
      return element;
    }

template< class BindingType>
  template< typename MemberType>
    detail::HeteroMapElement< BindingType, MemberType> *
    HeteroMap< BindingType>::element(const std::string & name,
        MemberType (BindingType::* const member))
    {
      detail::HeteroMapElement< BindingType, MemberType> * const element =
          new detail::HeteroMapElement< BindingType, MemberType>(
              getType< typename detail::StripOptional< MemberType>::Type>(),
              member);

      // WARNING: This will only insert the new entry if one with the same name doesn't
      // exist already.  Otherwise the new one will be destroyed and the old one returned.
      // This may not be what the user expects.
      myEntries.insert(name, element);
      return element;
    }

template< class BindingType>
  template< typename BaseType>
    void
    HeteroMap< BindingType>::extends()
    {
      typedef typename BaseType::BindingType BaseBinding;
      const BaseType base;
      BOOST_FOREACH(typename BaseType::EntriesMap::const_reference entry,
          boost::make_iterator_range(base.entriesBegin(), base.entriesEnd()))
      {
        myEntries.insert(entry.first,
            new detail::HeteroMapBaseEntryWrapper< BindingType, BaseBinding>(
                *entry.second));
      }
    }

} // namespace schemer

#endif /* SCHEMER_MAP_DETAIL_H */
