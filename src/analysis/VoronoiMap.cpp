/*
 * VoronoiMap.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "analysis/VoronoiMap.h"

#ifdef SSLIB_USE_CGAL

namespace sstbx {
namespace analysis {

void VoronoiMap::addData(const ::arma::vec2 & point, const ::std::string & tag)
{
  Site2 voronoiSite(point(0), point(1));

  ::std::pair<TagSet::iterator, bool> result = myPointTags[voronoiSite].insert(tag);


}


}
}

#endif /* SSLIB_USE_CGAL */
