///*
// * MultiRange.h
// *
// *
// *  Created on: Aug 17, 2011
// *      Author: Martin Uhrin
// */
//
//#ifndef MULTI_RANGE_H
//#define MULTI_RANGE_H
//
//// INCLUDES /////////////////////////////////////////////
//#include "spl/SSLib.h"
//
//#include <vector>
//
//#include <boost/iterator/iterator_facade.hpp>
//#include <boost/range/iterator_range.hpp>
//#include <boost/range/any_range.hpp>
//
//
//// FORWARD DECLARATIONS ////////////////////////////////////
//
//namespace spl {
//namespace utility {
//
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//class MultiRangeIterator;
//
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//class MultiRange : public ::boost::iterator_range<MultiRangeIterator<Value, Traversal, Reference, Difference> >
//{
//public:
//  typedef ::boost::any_range<Value, Traversal, Reference, Difference> AnyRange;
//  typedef typename AnyRange::iterator AnyIterator;
//
//  template <class Range>
//  void pushBack(const Range & range);
//
//private:
//  typedef ::std::vector<AnyRange> Ranges;
//
//  Ranges myRanges;
//
//  //friend class MultiRangeIterator<MultiRange>;
//
//};
////
////template <
////  class Value,
////  class Traversal,
////  class Reference,
////  class Difference
////>
////class MultiRangeAccess;
////
//template <
//  class Value,
//  class Traversal,
//  class Reference,
//  class Difference
//>
//class MultiRangeIterator :
//  public ::boost::iterator_facade<
//    MultiRangeIterator<Value, Traversal, Reference, Difference>,
//    Value,
//    boost::forward_traversal_tag
//  >
//{
//  typedef MultiRange<Value, Traversal, Reference, Difference> MultiRangeType;
//  typedef typename MultiRange<Value, Traversal, Reference, Difference>::AnyIterator AnyIterator;
//public:
//  typedef ::std::vector<AnyIterator> Iterators;
//
//  MultiRangeIterator(Iterators & iterators, const size_t currentIterating = 0);
//
//private:
// 
//
//  AnyIterator & getIterator(const size_t idx);
//  const AnyIterator & getIterator(const size_t idx) const;
//
//  Iterators myIterators;
//
//  //friend class MultiRangeAccess<Value, Traversal, Reference, Difference>;
//  friend class ::boost::iterator_core_access;
//};
////
////template <
////  class Value,
////  class Traversal,
////  class Reference,
////  class Difference
////>
////class MultiRangeAccess
////{
////  typedef MultiRange<Value, Traversal, Reference, Difference> MultiRangeType;
////  typedef MultiRangeIterator<Value, Traversal, Reference, Difference> MultiRangeIteratorType;
////public:
////  MultiRangeAccess(MultiRangeIteratorType & iterator);
////
////  Reference operator[](const size_t idx);
////
////private:
////  MultiRangeIteratorType myIterator;
////};
//
//} // namespace utility
//} // namespace spl
//
////#include "spl/utility/detail/MultiRange.h"
//
//#endif /* MULTI_RANGE_H */
