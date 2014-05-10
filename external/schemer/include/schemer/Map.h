/*
 * SchemaMap.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_MAP_H
#define SCHEMER_MAP_H

// INCLUDES /////////////////////////////////////////////
#include <map>
#include <string>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include "schemer/Assert.h"
#include "schemer/detail/Type.h"
#include "schemer/Scalar.h"

// DEFINES //////////////////////////////////////////////

namespace schemer {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace detail {
template< typename T>
  struct StripOptional
  {
    typedef T Type;
  };

template< typename T>
  struct StripOptional< boost::optional< T> >
  {
    typedef T Type;
  };

template< class MapBindingType, typename MapBindingMemberType,
    typename T = typename StripOptional< MapBindingMemberType>::Type>
  class HeteroMapElement;
template< class T>
  class HeteroMapElementBase;
template< class T>
  HeteroMapElementBase< T> *
  new_clone(const HeteroMapElementBase< T> & entry);
template< class T>
  class HomoMapElement;
}

template< typename KeyType, typename EntryType>
  class Map : public detail::Type<
      std::map< typename KeyType::BindingType, typename EntryType::BindingType> >
  {
    // TODO: Test this class and make sure it's doing the right thing
    typedef typename KeyType::BindingType KeyBinding;
    typedef typename EntryType::BindingType EntryBinding;
    typedef detail::HomoMapElement< EntryType> Element;
    SCHEMER_STATIC_ASSERT_MSG(
        (boost::is_base_of<Scalar<KeyBinding>, KeyType>::value),
        "Key must be a scalar schema type.");
  public:
    typedef std::map< KeyBinding, EntryBinding> BindingType;

    Map();
    virtual
    ~Map()
    {
    }

    virtual bool
    valueToNode(const BindingType & value, YAML::Node * const node) const;
    virtual bool
    nodeToValue(const YAML::Node & node, BindingType * const value,
        ParseLog * const log) const;
    virtual bool
    nodeToValue(const YAML::Node & node, BindingType * const value) const
    {
      return nodeToValue(node, value, NULL);
    }

    Element *
    element(const std::string & name);

    virtual Map *
    clone() const;

    bool
    areUnknownEntriesAllowed() const;
    void
    setAllowUnknownEntries(const bool allowUnknownEntries);

  private:
    typedef std::map< const KeyBinding, Element> EntriesMap;

    const Scalar<KeyBinding> & myKey;
    const EntryType & myEntry;
    EntriesMap myEntries;
    bool myAllowUnknownEntries;
  };

template< class BT>
  class HeteroMap : public detail::Type< BT>
  {
  public:
    typedef BT BindingType;
    typedef detail::HeteroMapElementBase< BindingType> Entry;
    typedef boost::ptr_map< const std::string, Entry> EntriesMap;
    typedef typename EntriesMap::const_iterator EntryIterator;

    virtual
    ~HeteroMap()
    {
    }

    EntryIterator
    entriesBegin() const;
    EntryIterator
    entriesEnd() const;

    virtual bool
    valueToNode(const BindingType & map, YAML::Node * const node) const;
    virtual bool
    nodeToValue(const YAML::Node & node, BindingType * const map,
        ParseLog * log) const;
    virtual bool
    nodeToValue(const YAML::Node & node, BindingType * const value) const
    {
      return nodeToValue(node, value, NULL);
    }

    template< typename ElementType, typename MemberType>
      detail::HeteroMapElement< BindingType, MemberType,
          typename ElementType::BindingType> *
      element(const std::string & name,
          MemberType (BindingType::* const member));

    template< typename MemberType>
      detail::HeteroMapElement< BindingType, MemberType> *
      element(const std::string & name,
          MemberType (BindingType::* const member));

    template< typename BaseType>
      void
      extends();

    virtual HeteroMap *
    clone() const;

  private:
    EntriesMap myEntries;
  };

}

#endif /* SCHEMER_MAP_H */
