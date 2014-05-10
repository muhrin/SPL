/*
 * Variant.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_VARIANT_DETAIL_H
#define SCHEMER_VARIANT_DETAIL_H

// INCLUDES /////////////////////////////////////////////

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {

struct _null
{
  typedef void * BindingType;
};

template< class ScalarType, class ListType, class MapType>
  class ValueToNodeVisitor : public boost::static_visitor< bool>
  {
  public:
    ValueToNodeVisitor(YAML::Node * const node, const ScalarType & scalar,
        const ListType & list, const MapType & map) :
        myNode(node), myScalar(scalar), myList(list), myMap(map)
    {
    }

    bool
    operator()(const typename ScalarType::BindingType & binding) const
    {
      return myScalar.valueToNode(binding, myNode);
    }

    bool
    operator()(const typename ListType::BindingType & binding) const
    {
      return myList.valueToNode(binding, myNode);
    }

    bool
    operator()(const typename MapType::BindingType & binding) const
    {
      return myMap.valueToNode(binding, myNode);
    }

  private:
    YAML::Node * const myNode;
    const ScalarType & myScalar;
    const ListType & myList;
    const MapType & myMap;
  };

template< class ScalarType, class ListType, class MapType>
  bool
  VariantScalarListMap< ScalarType, ListType, MapType>::valueToNode(
      const BindingType & binding, YAML::Node * const node) const
  {
    ValueToNodeVisitor< ScalarType, ListType, MapType> valueToNodeVisitor(node,
        myScalarType, myListType, myMapType);

    return ::boost::apply_visitor(valueToNodeVisitor, binding);
  }

template< class ScalarType, class ListType, class MapType>
  bool
  VariantScalarListMap< ScalarType, ListType, MapType>::nodeToValue(
      const YAML::Node & node, BindingType * const binding,
      ParseLog * const log) const
  {
    if(node.IsScalar())
    {
      // Make the variant contain the scalar binding type
      *binding = ScalarType::BindingType();
      return myListType.nodeToValue(node,
          ::boost::get< ScalarType::BindingType>(binding), log);
    }
    else if(node.IsSequence())
    {
      // Make the variant contain the list binding type
      *binding = ListType::BindingType();
      return myListType.nodeToValue(node,
          ::boost::get< ListType::BindingType>(binding), log);
    }
    else if(node.IsMap())
    {
      // Make the variant contain the list binding type
      *binding = MapType::BindingType();
      return myMapType.nodeToValue(node,
          ::boost::get< MapType::BindingType>(binding), log);
    }
  }

template< class ScalarType, class ListType, class MapType>
  VariantScalarListMap< ScalarType, ListType, MapType> *
  VariantScalarListMap< ScalarType, ListType, MapType>::clone() const
  {
    return new VariantScalarListMap(*this);
  }

template< class ListSchema, class MapSchema>
  bool
  VariantListMap< ListSchema, MapSchema>::valueToNode(
      const BindingType & binding, YAML::Node * const node) const
  {
    ValueToNodeVisitor< _null, ListSchema, MapSchema> valueToNodeVisitor(node,
        _null(), myListType, myMapType);

    return ::boost::apply_visitor(valueToNodeVisitor, binding);
  }

template< class ListSchema, class MapSchema>
  bool
  VariantListMap< ListSchema, MapSchema>::nodeToValue(const YAML::Node & node,
      BindingType * const binding, ParseLog * const log) const
  {
    if(node.IsSequence())
    {
      // Make the variant contain the list binding type
      *binding = typename ListSchema::BindingType();
      return myListType.nodeToValue(node,
          ::boost::get< typename ListSchema::BindingType>(binding), log);
    }
    else if(node.IsMap())
    {
      // Make the variant contain the list binding type
      *binding = typename MapSchema::BindingType();
      return myMapType.nodeToValue(node,
          ::boost::get< typename MapSchema::BindingType>(binding), log);
    }
    else
      return false;

    return true;
  }

template< class ListSchema, class MapSchema>
  VariantListMap< ListSchema, MapSchema> *
  VariantListMap< ListSchema, MapSchema>::clone() const
  {
    return new VariantListMap(*this);
  }

}

#endif /* SCHEMER_VARIANT_DETAIL_H */
