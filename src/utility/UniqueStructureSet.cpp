/*
 * UniqueStructureSet.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "utility/UniqueStructureSet.h"

#include <boost/foreach.hpp>

#include "utility/IStructureComparator.h"


namespace sstbx {
namespace utility {


// StructureMetadata implementation ///////

UniqueStructureSet::StructureMetadata::StructureMetadata():
timesFound(1)
{}

// UniqueStructureSet implementation

UniqueStructureSet::UniqueStructureSet(const IStructureComparator & comparator):
myComparator(comparator.generateBuffered())
{}

UniqueStructureSet::iterator
UniqueStructureSet::begin()
{
  return iterator(myStructures.begin(), TakeFirst());
}

UniqueStructureSet::const_iterator
UniqueStructureSet::begin() const
{
  return const_iterator(myStructures.begin(), TakeFirstConst());
}

UniqueStructureSet::iterator
UniqueStructureSet::end()
{
  return iterator(myStructures.end(), TakeFirst());
}

UniqueStructureSet::const_iterator
UniqueStructureSet::end() const
{
  return const_iterator(myStructures.end(), TakeFirstConst());
}

UniqueStructureSet::reverse_iterator
UniqueStructureSet::rbegin()
{
  return reverse_iterator(myStructures.rbegin(), TakeFirst());
}

UniqueStructureSet::const_reverse_iterator
UniqueStructureSet::rbegin() const
{
  return const_reverse_iterator(myStructures.rbegin(), TakeFirstConst());
}

UniqueStructureSet::reverse_iterator
UniqueStructureSet::rend()
{
  return reverse_iterator(myStructures.rend(), TakeFirst());
}

UniqueStructureSet::const_reverse_iterator
UniqueStructureSet::rend() const
{
  return const_reverse_iterator(myStructures.rend(), TakeFirstConst());
}

bool UniqueStructureSet::empty() const
{
  return myStructures.empty();
}

UniqueStructureSet::size_type
UniqueStructureSet::size() const
{
  return myStructures.size();
}

UniqueStructureSet::size_type
UniqueStructureSet::max_size() const
{
  return myStructures.max_size();
}

UniqueStructureSet::insert_return_type
UniqueStructureSet::insert(common::Structure * const newStructure)
{
  MapInsertReturn pair = insertStructure(newStructure);
  return insert_return_type(iterator(pair.first), pair.second);
}

void UniqueStructureSet::erase(UniqueStructureSet::iterator position)
{
  myComparator->releaseComparisonDataFor(*(*position));
  myStructures.erase(position.base());
}

//
//const std::pair<sstbx::common::Structure *, bool>
//UniqueStructureSet::insert(sstbx::common::Structure * const str)
//{
//	sstbx::common::Structure * similarTo = NULL;
//	bool unique = true;
//  BOOST_FOREACH(StructureMap::value_type strDat, myStructures)
//	{
//		if(myComparator->areSimilar(*str, *strDat.first))
//		{
//      ++strDat.second.timesFound;
//			similarTo = strDat.first;
//			unique = false;
//			break;
//		}
//	}
//
//	// If it is not like any of those we have already then insert it
//	if(unique)
//	{
//		myStructures.insert(
//      StructureMap::value_type(str, StructureMetadata())
//		);
//	}
//  else
//  {
//    // Tell the comparator the remove the comparison data for this structure as we
//    // are not storing it.
//    myComparator->releaseComparisonDataFor(*str);
//  }
//
//	return ReturnPair(unique ? str : similarTo, unique);
//}

void UniqueStructureSet::clear()
{
  BOOST_FOREACH(StructureMap::value_type & strDat, myStructures)
	{
		myComparator->releaseComparisonDataFor(*strDat.first);
	}
	myStructures.clear();
}

UniqueStructureSet::MapInsertReturn
UniqueStructureSet::insertStructure(common::Structure * const str)
{
  SSLIB_PRECONDITION(str);

  MapInsertReturn returnPair;
  // .second is used to indiate that a new structure was inserted
  returnPair.second = true;

  // Check if we have a structure like this already
  for(StructureMap::iterator it = myStructures.begin(), end = myStructures.end();
    it != end; ++it)
  {
		if(myComparator->areSimilar(*str, *it->first))
		{
      ++it->second.timesFound;
      returnPair.first = it;
      returnPair.second = false;
			break;
		}
  }

	// If it is not like any of those we have already then insert it
	if(returnPair.second)
	{
		returnPair = myStructures.insert(StructureMap::value_type(str, StructureMetadata()));
	}
  else
  {
    // Tell the comparator the remove the comparison data for this structure as we
    // are not storing it.
    myComparator->releaseComparisonDataFor(*str);
  }

	return returnPair;
}

void UniqueStructureSet::eraseStructure(common::Structure * const str)
{
  SSLIB_PRECONDITION(str);


  
}

}
}
