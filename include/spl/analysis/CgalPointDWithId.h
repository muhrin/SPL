/*
 * CgalPointDWithId.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#ifndef CGAL_POINT_D_WITH_ID_H
#define CGAL_POINT_D_WITH_ID_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <CGAL/Kernel_d/Point_d.h>
#include <CGAL/Origin.h>

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {

template <class Kernel>
class CgalPointDWithId : public CGAL::Point_d<Kernel>
{
  typedef Kernel R;
  typedef CgalPointDWithId<Kernel> Self;
  typedef typename CGAL::Point_d<Kernel> Base;
  typedef typename Kernel::RT RT;
public:

  typedef int Id;

  CgalPointDWithId():
    myId(-1)
  {}

  CgalPointDWithId(const Self & toCopy):
    Base(toCopy),
    myId(toCopy.myId)
  {}

  explicit CgalPointDWithId(const Base & p):
    Base(p),
    myId(-1)
  {}

  CgalPointDWithId(int d, CGAL::Origin o):
    Base(d, o),
    myId(-1)
  {}

  template <class InputIterator>
  CgalPointDWithId(int d, InputIterator first, InputIterator last):
    Base(d, first, last),
    myId(-1)
  {}

  template <class InputIterator>
  CgalPointDWithId(int d, InputIterator first, InputIterator last, RT D):
    Base(d, first, last, D),
    myId(-1)
  {}

  CgalPointDWithId(RT x, RT y, RT w = 1):
    Base(x, y, w),
    myId(-1)
  {}

  CgalPointDWithId(RT x, RT y, RT z, RT w):
    Base(x, y, z, w),
    myId(-1)
  {}

  Self operator +(const CGAL::Vector_d<R> & v) const
  {
    Self p(*this);
    return p += v;
  }

  Self& operator+=(const CGAL::Vector_d<R>& v)
  { return static_cast<Self&>(Base::operator+=(v)); }
  Self& operator-=(const CGAL::Vector_d<R>& v)
  { return static_cast<Self&>(Base::operator-=(v)); }

  const Id & getId() const
  { return myId; }
  void setId(const Id & id)
  { myId = id; }

private:
  Id myId;
};

template <class Kernel>
CgalPointDWithId<Kernel> operator +(const CGAL::Origin & o, const CGAL::Vector_d<Kernel> & v)
{
  CGAL::Point_d<Kernel> p(CGAL::operator +(o, v));
  return CgalPointDWithId<Kernel>(p);
}

}
}


#endif // SPL_WITH_CGAL
#endif /* CGAL_POINT_D_WITH_ID_H */
