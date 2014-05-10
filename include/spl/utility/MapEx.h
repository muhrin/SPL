/*
 * MapEx.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MAP_EX_H
#define MAP_EX_H

// INCLUDES /////////////////////////////////////////////
#include <stdexcept>
#include <map>

namespace spl {
namespace utility {

template <typename Key, typename T>
class MapEx : public ::std::map<Key, T>
{
  typedef ::std::map<Key, T> base_t;
public:

  // Typedefs from map ///////////////
  typedef Key key_type;
  typedef T mapped_type;
  typedef typename base_t::value_type value_type;
  typedef typename base_t::value_compare value_compare;
  typedef typename base_t::allocator_type allocator_type;
  typedef typename base_t::reference reference;
  typedef typename base_t::const_reference const_reference;
  typedef typename base_t::iterator iterator;
  typedef typename base_t::const_iterator const_iterator;
  typedef typename base_t::size_type size_type;
  typedef typename base_t::difference_type difference_type;
  typedef typename base_t::pointer pointer;
  typedef typename base_t::const_pointer const_pointer;
  typedef typename base_t::reverse_iterator reverse_iterator;
  typedef typename base_t::const_reverse_iterator const_reverse_iterator;

  const T & operator ()(const Key & key) const;
};

template <typename Key, typename T>
const T & MapEx<Key, T>::operator ()(const Key & key) const
{
  const_iterator it = this->find(key);

  if(it == base_t::end())
  {
    throw ::std::runtime_error("Attempting to access value with key that does not exist");
  }
  return it->second;
}

}
}

#endif /* MAP_EX_H */
