/*
 * NamedProperty.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef NAMED_PROPERTY_H
#define NAMED_PROPERTY_H

// INCLUDES /////////////////////////////////////////////

#include <map>
#include <string>
#include <ostream>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "utility/HeterogeneousMap.h"
#include "utility/TransformFunctions.h"

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx {
namespace utility {

template <typename Container>
class NamedProperty
{
public:
  virtual ~NamedProperty() {}

  virtual ::std::string getName() const = 0;
  virtual ::boost::optional< ::std::string> getValue(const Container & container) const = 0;
  virtual bool setValue(Container & container, const ::std::string & valueString) = 0;
};

template <typename T>
class NamedKey : public Key<T>, public NamedProperty<HeterogeneousMap>
{
public:
  NamedKey(const ::std::string & name);

  // From NamedProperty //////////////////
  virtual ::std::string getName() const;
  virtual ::boost::optional< ::std::string> getValue(const HeterogeneousMap & container) const;
  virtual bool setValue(HeterogeneousMap & container, const ::std::string & valueString);
  // End from NamedProperty //////////////

private:

  const ::std::string myName;
};

template <typename Container>
class NamedPropertyStore
{
private:
  typedef ::std::map< ::std::string, NamedProperty<Container> *> Properties;
public:

  typedef ::boost::transform_iterator<
    utility::TakeSecondDereferenced< ::std::string, NamedProperty<Container> *, NamedProperty<Container> & >,
    typename Properties::iterator> iterator;
  typedef ::boost::transform_iterator<
    utility::TakeSecondDereferenced< ::std::string, NamedProperty<Container> *, NamedProperty<Container> & >,
    typename Properties::const_iterator> const_iterator;

  bool addProperty(NamedProperty<Container> & namedProperty);
  bool removeProperty(NamedProperty<Container> & namedProperty);
  NamedProperty<Container> * getProperty(const ::std::string & name);

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

private:

  Properties myProperties;
};

}
}

#include "utility/detail/NamedProperty.h"

#endif /* NAMED_PROPERTY_H */
