/*
 * MatplotlibMapOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef MATPLOTLIB_MAP_OUTPUTTER_H
#define MATPLOTLIB_MAP_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <map>

#include <boost/range/iterator_range.hpp>

#include "spl/analysis/ArrangementMapOutputter.h"

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename MapTraits>
  class MatplotlibMapOutputter : public ArrangementMapOutputter< MapTraits>
  {
  public:
    typedef typename MapTraits::Label Label;
    typedef typename MapTraits::Arrangement Arrangement;

  private:
    typedef std::map< Label,
        typename ArrangementMapOutputter< MapTraits>::LabelProperties> LabelProperties;

  public:
    virtual
    ~MatplotlibMapOutputter()
    {
    }

    virtual void
    outputArrangement(const Arrangement & map, std::ostream * const os) const;
    virtual void
    outputArrangement(const Arrangement & map,
        const LabelProperties & labelProperties, std::ostream * const os) const;
    virtual std::string
    fileExtension() const;

  private:
    typedef typename MapTraits::ArrTraits::Point_2 Point;
    typedef typename Arrangement::Face Face;
    typedef typename MapTraits::Bezier Bezier;
    typedef std::map< Label, std::string> ColourMap;

    class FacePath;

    static const std::string PATH;
    static const std::string PATH_PATCH;
    static const std::string INDENT;
    static const std::string PLOT;

    void
    out(const Arrangement & map, const LabelProperties * const labelProperties,
        std::ostream * const os) const;
    void
    drawFace(const Face & face, std::ostream * const os,
        const std::string & label, const std::string & colour) const;
    ColourMap
    getColourMap(const LabelProperties * const labelProperties,
        const boost::iterator_range< typename Arrangement::Face_const_iterator> & faces) const;
    void
    printProperties(const LabelProperties * const properties,
        const ColourMap & colourMap, std::ostream * const os) const;
    std::string
    toHexString(const int colour) const;
    void
    drawText(const Point pt, const std::string & label,
        std::ostream * const os) const;
    int
    rgbColour(const int red, const int green, const int blue) const;
  };

}
}

#include "spl/analysis/detail/MatplotlibMapOutputter.h"

#endif // SPL_USE_CGAL
#endif /* MATPLOTLIB_MAP_OUTPUTTER_H */
