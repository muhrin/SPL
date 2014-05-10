/*
 * TypedDataTable.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


#ifndef TYPED_DATA_TABLE_DETAIL_H
#define TYPED_DATA_TABLE_DETAIL_H

// INCLUDES /////////////////////////////////////////////
#include <functional>
#include <sstream>

#include <boost/iterator/indirect_iterator.hpp>

namespace spl {
namespace utility {

template <typename TableKey>
Column<TableKey>::Column(const ::std::string & name):
myName(name)
{}

template <typename TableKey>
const ::std::string & Column<TableKey>::getName() const
{
  return myName;
}

template <typename T, typename TableKey>
TypedColumn<T, TableKey>::TypedColumn(const ::std::string & name):
Column<TableKey>(name)
{}

template <typename T, typename TableKey>
::boost::optional< ::std::string>
TypedColumn<T, TableKey>::getValue(
  const TypedDataTable<TableKey> & table,
  const TableKey & tableKey) const
{
  const T * const value = table.get(tableKey, *this);

  if(!value)
    return ::boost::optional< ::std::string>();

  ::std::stringstream ss;
  ss << *value;
  return ss.str();
}

template <typename T, typename TableKey>
bool TypedColumn<T, TableKey>::feedFormatter(
  ::boost::format & formatter,
  const TypedDataTable<TableKey> & table,
  const TableKey & tableKey) const
{
  const T * const value = table.get(tableKey, *this);

  if(!value)
    return false;

  formatter % *value;

  return true;
}

template <typename T, typename TableKey>
typename TypedColumn<T, TableKey>::MapKey &
TypedColumn<T, TableKey>::getKey()
{
  return myKey;
}

template <typename T, typename TableKey>
const typename TypedColumn<T, TableKey>::MapKey &
TypedColumn<T, TableKey>::getKey() const
{
  return myKey;
}

template <typename Key>
template <typename T>
void TypedDataTable<Key>::set(const Key & key, TypedColumn<T, Key> & column, const T & value)
{
  myTable[key][column.getKey()] = value;
}

template <typename Key>
template <typename T>
T * TypedDataTable<Key>::get(const Key & key, const TypedColumn<T, Key> & column)
{
  const typename Table::iterator rowIt = myTable.find(key);

  if(rowIt == myTable.end())
    return NULL;

  return rowIt->second.find(column.getKey());
}

template <typename Key>
template <typename T>
const T * TypedDataTable<Key>::get(const Key & key, const TypedColumn<T, Key> & column) const
{
  const typename Table::const_iterator rowIt = myTable.find(key);

  if(rowIt == myTable.end())
    return NULL;

  return rowIt->second.find(column.getKey());
}

template <typename Key>
typename TypedDataTable<Key>::RowIterator
TypedDataTable<Key>::beginRows()
{
  return myTable.begin();
}

template <typename Key>
typename TypedDataTable<Key>::RowIterator
TypedDataTable<Key>::endRows()
{
  return myTable.end();
}

template <typename Key>
typename TypedDataTable<Key>::RowIterator
TypedDataTable<Key>::findRow(const Key & key)
{
  return myTable.find(key);
}

template <typename Key>
void TypedDataTable<Key>::eraseRow(RowIterator pos)
{
  return myTable.erase(pos);
}

template <typename Key>
void TypedDataTable<Key>::eraseRow(const Key & key)
{
  myTable.erase(myTable.find(key));
}


template <typename Key>
template <typename T>
void TypedDataTable<Key>::getAscending(SortedKeys & sortedKeys, const TypedColumn<T, Key> & column) const
{
  typedef detail::ColumnComparator<T, Key> Comparator;

  // Empty the container and copy all the current keys
  sortedKeys.clear();
  sortedKeys.reserve(myTable.size());
  typename SortedKeys::iterator insertPoint = sortedKeys.end();
  BOOST_FOREACH(const typename Table::value_type & entry, myTable)
  {
    if(get(entry.first, column))
      insertPoint = sortedKeys.insert(insertPoint, entry.first) + 1;
    else
      insertPoint = sortedKeys.insert(insertPoint, entry.first);
  }

  // Sort using the custom comparator
  ::std::sort(sortedKeys.begin(), insertPoint, Comparator(*this, column));
}

template <typename Key>
template <typename T>
void TypedDataTable<Key>::getDescending(SortedKeys & sortedKeys, const TypedColumn<T, Key> & column) const
{
  typedef detail::ColumnComparator<T, Key> Comparator;

  // Empty the container and copy all the current keys
  sortedKeys.clear();
  sortedKeys.reserve(myTable.size());
  typename SortedKeys::iterator insertPoint = sortedKeys.end();
  BOOST_FOREACH(const typename Table::value_type & entry, myTable)
  {
    if(get(entry.first, column))
      insertPoint = sortedKeys.insert(insertPoint, entry.first) + 1;
    else
      insertPoint = sortedKeys.insert(insertPoint, entry.first);
  }

  // Sort using the custom comparator
  ::std::sort(sortedKeys.begin(), insertPoint, Comparator(*this, column, true));
}

template <typename Key>
size_t TypedDataTable<Key>::size() const
{
  return myTable.size();
}

namespace detail {

template <typename T, typename TableKey>
class ColumnComparator : public ::std::binary_function<const TableKey &, const TableKey &, bool>
{
public:
  ColumnComparator(const TypedDataTable<TableKey> & table, const TypedColumn<T, TableKey> & column);
  ColumnComparator(const TypedDataTable<TableKey> & table, const TypedColumn<T, TableKey> & column, const bool reverseComparison);

  bool operator()(const TableKey & key1, const TableKey & key2);
private:
  const TypedDataTable<TableKey> & myTable;
  const TypedColumn<T, TableKey> & myColumn;
  const bool myReverseComparison;
};

template <typename T, typename TableKey>
ColumnComparator<T, TableKey>::ColumnComparator(
  const TypedDataTable<TableKey> & table,
  const TypedColumn<T, TableKey> & column):
myTable(table),
myColumn(column),
myReverseComparison(false)
{}

template <typename T, typename TableKey>
ColumnComparator<T, TableKey>::ColumnComparator(
  const TypedDataTable<TableKey> & table,
  const TypedColumn<T, TableKey> & column,
  const bool reverseComparison):
myTable(table),
myColumn(column),
myReverseComparison(reverseComparison)
{}

template <typename T, typename TableKey>
bool ColumnComparator<T, TableKey>::operator()(const TableKey & key1, const TableKey & key2)
{
  const T * const v1 = myTable.get(key1, myColumn);
  const T * const v2 = myTable.get(key2, myColumn);

  if(!v1 || !v2)
    return false; // Can't compare as we don't have one or more values

  // NOTE: Need to use < and > operators as we need a comparator that produces strict weak ordering
  return myReverseComparison ? *v1 > *v2 : *v1 < *v2;
}


}

}
}

#endif /* TYPED_DATA_TABLE_DETAIL_H */
