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

namespace sstbx {
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
template <typename T>
void TypedDataTable<Key>::getAscending(SortedKeys & sortedKeys, const TypedColumn<T, Key> & column) const
{
  typedef detail::ColumnComparator<T, Key> Comparator;

  // Empty the container and copy all the current keys
  sortedKeys.clear();
  BOOST_FOREACH(const typename Table::value_type & entry, myTable)
  {
    sortedKeys.push_back(entry.first);
  }

  // Sort using the custom comparator
  ::std::sort(sortedKeys.begin(), sortedKeys.end(), Comparator(*this, column));
}

namespace detail {

template <typename T, typename TableKey>
class ColumnComparator : public ::std::binary_function<const TableKey &, const TableKey &, bool>
{
public:

  ColumnComparator(const TypedDataTable<TableKey> & table, const TypedColumn<T, TableKey> & column);

  bool operator()(const TableKey & key1, const TableKey & key2);
private:
  const TypedDataTable<TableKey> & myTable;
  const TypedColumn<T, TableKey> & myColumn;
};

template <typename T, typename TableKey>
ColumnComparator<T, TableKey>::ColumnComparator(
  const TypedDataTable<TableKey> & table,
  const TypedColumn<T, TableKey> & column):
myTable(table),
myColumn(column)
{}

template <typename T, typename TableKey>
bool ColumnComparator<T, TableKey>::operator()(const TableKey & key1, const TableKey & key2)
{
  const T * const v1 = myTable.get(key1, myColumn);
  const T * const v2 = myTable.get(key2, myColumn);

  if(v1 && v2)
    return *v1 < *v2;

  return key1 < key2;
}


}

}
}

#endif /* TYPED_DATA_TABLE_DETAIL_H */
