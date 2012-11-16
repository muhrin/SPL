/*
 * UniqueStructureSet.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef UNIQUE_STRUCTURE_SET_H
#define UNIQUE_STRUCTURE_SET_H

// INCLUDES /////////////////////////////////////////////
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "common/Structure.h"
#include "utility/IBufferedComparator.h"
#include "utility/TransformFunctions.h"


// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace utility {
class IStructureComparator;
}
}


namespace sstbx {
namespace utility {

namespace utility_detail {



} // namespace utility_detail

class UniqueStructureSet
{
public:

	struct StructureMetadata
	{
    StructureMetadata();

		/** The number of times this structure has been found */
		size_t				                  timesFound;
	};

private:

	typedef std::map<sstbx::common::Structure *, StructureMetadata> StructureMap;

public:

  typedef utility::TakeFirst<common::Structure *, StructureMetadata> TakeFirst;
  typedef utility::TakeFirst<const common::Structure *, StructureMetadata> TakeFirstConst;

  typedef common::Structure * value_type;
  typedef boost::transform_iterator<TakeFirst, StructureMap::iterator> iterator;
  typedef boost::transform_iterator<TakeFirstConst, StructureMap::const_iterator> const_iterator;
  typedef boost::transform_iterator<TakeFirst, StructureMap::reverse_iterator> reverse_iterator;
  typedef boost::transform_iterator<TakeFirstConst, StructureMap::const_reverse_iterator> const_reverse_iterator;
  typedef std::pair<iterator, bool> insert_return_type;

  typedef StructureMap::size_type size_type;

	typedef std::pair<sstbx::common::Structure *, bool> ReturnPair;

	UniqueStructureSet(const IStructureComparator & comparator);

  // Iterators /////////////////////////
  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;

  reverse_iterator rend();
  const_reverse_iterator rend() const;

  // Capacity ////////////////////////////
  bool empty() const;
  size_type size() const;
  size_type max_size() const;

  // Modifiers ///////////////////////////
  insert_return_type insert(common::Structure * const newStructure);

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last);
  void erase(iterator position);
  
	void clear();

private:

  typedef std::pair<StructureMap::iterator, bool> MapInsertReturn;

  const ::boost::shared_ptr<IBufferedComparator> myComparator;

  MapInsertReturn insertStructure(common::Structure * const str);
  void eraseStructure(common::Structure * const str);

	StructureMap myStructures;
};

template <class InputIterator>
void UniqueStructureSet::insert(const InputIterator first, const InputIterator last)
{
  // Insert any unique structures in the range
  for(InputIterator it = first; it != last; ++it)
    insertStructure(*it);
}

}
}


#endif /* UNIQUE_STRUCTURE_SET_H */
