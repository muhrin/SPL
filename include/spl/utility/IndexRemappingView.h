/*
 * IndexRemappingView.h
 *
 *
 *  Created on: Sep 12, 2012
 *      Author: Martin Uhrin
 */

#ifndef INDEX_REMAPPING_VIEW_H
#define INDEX_REMAPPING_VIEW_H

// INCLUDES /////////////////////////////////////////////

#include <vector>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template <typename Object, typename Element>
class IndexRemappingView
{
public:

  IndexRemappingView(Object & object):
    myObject(object)
    {}

  virtual ~IndexRemappingView() {}

  Object & getObject();
  const Object & getObject() const;

  Element & operator [](const size_t idx);
  const Element & operator [](const size_t idx) const;

protected:

  virtual ptrdiff_t getMappedIndex(const size_t idx) const = 0;
  
  Object &                 myObject;

};

template <typename Object, typename Element>
class IndexRemappingViewShared : public IndexRemappingView<Object, Element>
{
public:

  IndexRemappingViewShared(Object & object, const ::std::vector<ptrdiff_t> & mapping):
    IndexRemappingView<Object, Element>(object),
    myMapping(mapping)
    {}

  IndexRemappingViewShared(const IndexRemappingViewShared<Object, Element> & toCopy):
    IndexRemappingView<Object, Element>(toCopy.myObject),
    myMapping(toCopy.myMapping)
  {}

  virtual ptrdiff_t getMappedIndex(const size_t idx) const
  {
    return myMapping[idx];
  }

private:

  const ::std::vector<ptrdiff_t> & myMapping;
};


// IMPLEMENTATION /////////////////////////////////////////////


template <typename Object, typename Element>
Object & IndexRemappingView<Object, Element>::getObject()
{
  return myObject;
}

template <typename Object, typename Element>
const Object & IndexRemappingView<Object, Element>::getObject() const
{
  return myObject;
}

template <typename Object, typename Element>
Element & IndexRemappingView<Object, Element>::operator [](const size_t idx)
{
  return myObject[getMappedIndex(idx)];
}

template <typename Object, typename Element>
const Element & IndexRemappingView<Object, Element>::operator [](const size_t idx) const
{
  return myObject[getMappedIndex(idx)];
}


}
}

#endif /* INDEX_REMAPPING_VIEW_H */
