/*
 * DataTable.h
 *
 *
 *  Created on: Jul 9, 2014
 *      Author: Martin Uhrin
 */

#ifndef SPL__UTILITY__DATA_TABLE_H_
#define SPL__UTILITY__DATA_TABLE_H_

// INCLUDES /////////////////////////////////////////////

#include <map>
#include <string>
#include <utility>
#include <vector>

#ifdef SPIPE_USE_BOOST_THREAD
#  include <boost/thread/mutex.hpp>
#endif

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template< typename K, typename R>
  class TypedAssociativeTable
  {
  public:
    typedef K Key;
    typedef R Row;

  private:
    typedef std::map< Key, Row> Rows;

  public:
    typedef typename Rows::iterator iterator;
    typedef typename Rows::const_iterator const_iterator;
    typedef typename Rows::value_type value_type;
    typedef typename Rows::reference reference;
    typedef typename Rows::const_reference const_reference;

    static TypedAssociativeTable< K, R>
    load(const std::string & filename)
    {
      return load(filename, " ,");
    }

    template< typename Char>
    static TypedAssociativeTable< K, R>
    load(const std::string & filename, const Char * const delimiter);

    iterator
    begin();
    iterator
    end();

    const_iterator
    begin() const;
    const_iterator
    end() const;

    std::pair< iterator, bool>
    insert(const Key & key, const Row & row);
    Row &
    operator [](const Key & key);
    void
    erase(iterator row);
    iterator
    find(const Key & key);
    const_iterator
    find(const Key & key) const;

    bool
    empty() const;

  private:
    Rows myRows;
  };

}
}

#include "spl/utility/detail/DataTable.h"

#endif /* SPL__UTILITY__DATA_TABLE_H_ */
