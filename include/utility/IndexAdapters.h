/*
 * IndexAdapters.h
 *
 *
 *  Created on: Oct 30, 2012
 *      Author: Martin Uhrin
 */

#ifndef INDEX_ADAPTERS_H
#define INDEX_ADAPTERS_H

// INCLUDES /////////////////////////////////////////////

namespace sstbx {
namespace utility {

template <typename Index = size_t>
class StridedIndexAdapter
{
public:
  StridedIndexAdapter(const Index stride):
  myStride(stride)
  {}

  Index operator ()(const Index idx) const
  { return idx / myStride; }

  Index inv(const Index idx) const
  { return idx * myStride; }

private:
  const size_t myStride;
};

}
}

#endif /* INDEX_ADAPTERS_H */
