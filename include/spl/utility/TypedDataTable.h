/*
 * TypedDataTable.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


#ifndef TYPED_DATA_TABLE_H
#define TYPED_DATA_TABLE_H

// INCLUDES /////////////////////////////////////////////

#include <vector>

#include <boost/format.hpp> 
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include "spl/utility/HeterogeneousMap.h"

namespace spl {
namespace utility {

// FORWARD DECLARES /////////////////////
template <typename Key>
class TypedDataTable;

namespace detail {
template <typename T, typename TableKey>
class ColumnComparator;
}

template <typename TableKey>
class Column : private ::boost::noncopyable
{
public:
  Column(const ::std::string & name);
  virtual ~Column() {}

  const ::std::string & getName() const;
  
  virtual ::boost::optional< ::std::string> getValue(
    const TypedDataTable<TableKey> & table,
    const TableKey & tableKey) const = 0;

  virtual bool feedFormatter(
    ::boost::format & formatter,
    const TypedDataTable<TableKey> & table,
    const TableKey & tableKey) const = 0;

private:
  const ::std::string myName;
};

template <typename T, typename TableKey>
class TypedColumn : public Column<TableKey>
{
public:

  TypedColumn(const ::std::string & name);
  virtual ~TypedColumn() {}

  virtual ::boost::optional< ::std::string> getValue(
    const TypedDataTable<TableKey> & table,
    const TableKey & tableKey) const;

  virtual bool feedFormatter(
    ::boost::format & formatter,
    const TypedDataTable<TableKey> & table,
    const TableKey & tableKey) const;

private:

  typedef Key<T> MapKey;

  MapKey & getKey();
  const MapKey & getKey() const;

  MapKey myKey;

  friend class TypedDataTable<TableKey>;
};

template <typename Key>
class TypedDataTable : private ::boost::noncopyable
{
  typedef ::spl::utility::HeterogeneousMap Row;
  typedef ::std::map<Key, Row> Table;
public:
  typedef ::std::vector<Key> SortedKeys;
  typedef typename Table::iterator RowIterator;

  template <typename T>
  void set(const Key & key, TypedColumn<T, Key> & column, const T & value);

  template <typename T>
  T * get(const Key & key, const TypedColumn<T, Key> & column);

  template <typename T>
  const T * get(const Key & key, const TypedColumn<T, Key> & column) const;

  RowIterator beginRows();
  RowIterator endRows();
  RowIterator findRow(const Key & key);
  void eraseRow(RowIterator pos);
  void eraseRow(const Key & key);

  template <typename T>
  void getAscending(SortedKeys & sortedKeys, const TypedColumn<T, Key> & column) const;

  template <typename T>
  void getDescending(SortedKeys & sortedKeys, const TypedColumn<T, Key> & column) const;

  size_t size() const;

private:
  Table myTable;
};

}
}

#include "spl/utility/detail/TypedDataTable.h"

#endif /* TYPED_DATA_TABLE_H */
