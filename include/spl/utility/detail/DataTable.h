/*
 * DataTable.h
 *
 *
 *  Created on: Jul 9, 2014
 *      Author: Martin Uhrin
 */

#ifndef SPL__UTILITY__DETAIL__DATA_TABLE_H_
#define SPL__UTILITY__DETAIL__DATA_TABLE_H_

// INCLUDES /////////////////////////////////////////////
#include <string>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "spl/SSLibAssert.h"

namespace spl {
namespace utility {

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::iterator
  TypedAssociativeTable< K, R>::begin()
  {
    return myRows.begin();
  }

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::iterator
  TypedAssociativeTable< K, R>::end()
  {
    return myRows.end();
  }

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::const_iterator
  TypedAssociativeTable< K, R>::begin() const
  {
    return myRows.begin();
  }

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::const_iterator
  TypedAssociativeTable< K, R>::end() const
  {
    return myRows.end();
  }

template< typename K, typename R>
  std::pair< typename TypedAssociativeTable< K, R>::iterator, bool>
  TypedAssociativeTable< K, R>::insert(const Key & key, const Row & row)
  {
    return myRows.insert(std::make_pair(key, row));
  }

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::Row &
  TypedAssociativeTable< K, R>::operator [](const Key & key)
  {
    typename Rows::iterator it = myRows.find(key);
    SSLIB_ASSERT(it != myRows.end());
    return it->second;
  }

template< typename K, typename R>
  void
  TypedAssociativeTable< K, R>::erase(iterator row)
  {
    myRows.erase(row);
  }

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::iterator
  TypedAssociativeTable< K, R>::find(const Key & key)
  {
    return myRows.find(key);
  }

template< typename K, typename R>
  typename TypedAssociativeTable< K, R>::const_iterator
  TypedAssociativeTable< K, R>::find(const Key & key) const
  {
    return myRows.find(key);
  }

template< typename K, typename R>
  bool
  TypedAssociativeTable< K, R>::empty() const
  {
    return myRows.empty();
  }

template< typename K, typename R>
template< typename Char>
TypedAssociativeTable< K, R>
TypedAssociativeTable< K, R>::load(const std::string & filename, const Char * const delimiter)
{
  std::ifstream file(filename.c_str());
  TypedAssociativeTable< K, R> table;

  if(file.is_open())
  {
    const boost::char_separator< char> sep(delimiter);
    std::string line;
    std::vector< std::string> strings;
    while(std::getline(file, line))
    {
      boost::split(strings, line, boost::is_any_of(delimiter));
      if(!strings.empty())
      {
        const K key = boost::lexical_cast< K>(strings[0]);
        strings.erase(strings.begin());
        const R & row = R::fromStrings(strings);
        table.insert(key, row);
      }
    }

    file.close();
  }

  return table;
}

}
}

#endif /* SPL__UTILITY__DETAIL__DATA_TABLE_H_ */
