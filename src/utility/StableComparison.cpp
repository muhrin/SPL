/*
 * StableComparison.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "utility/StableComparison.h"

#include <cmath>

namespace sstbx
{
  namespace utility
  {
    namespace StableComp
    {

      bool lt(const double v1,
              const double v2,
              const double prec)
      {
        return ( v1 < (v2 - prec) );
      }

      bool gt(const double v1,
              const double v2,
              const double prec)
      {
        return ( v2 < (v1 - prec) );
      }

      bool eq(const double v1,
              const double v2,
              const double prec)
      {
        return (!(lt(v1,v2,prec) ||
                  lt(v2,v1,prec) ) );
      }

      bool neq(const double v1,
               const double v2,
               const double prec)
      {
        return (!(eq(v1,v2,prec)));
      }

      bool leq(const double v1,
               const double v2,
               const double prec)
      {
        return (!gt(v1, v2, prec));
      }

      bool geq(const double v1,
               const double v2,
               const double prec)
      {
        return (!lt(v1, v2, prec));
      }

      double sign(const double v)
      {
        // consider 0 to be positive
        if (v >= 0) return 1.0;
        else return -1.0;
      }

      double round(const double v,
                   const int dec)
      {
        const double shift = std::pow(10.0, dec);
        const double shifted = v * shift;
        return std::floor(shifted+0.5) / shift;
      }

    }
  }
}
