/*
 * MatplotLibMapOutputter.h
 *
 *  Created on: Nov 11, 2013
 *      Author: Martin Uhrin
 */

#ifndef MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H
#define MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H

// INCLUDES ///////////////////
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <CGAL/centroid.h>
#include <CGAL/Polygon_2.h>

#include <spl/math/Random.h>

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename MapTraits>
  class MatplotlibMapOutputter< MapTraits>::FacePath
  {
    typedef typename Arrangement::X_monotone_curve_2 Curve;

  public:
    explicit
    FacePath(std::ostream * const os, const std::string & colour) :
        myOs(os), myColour(colour)
    {
      *myOs << "face_data = [\n";
    }
    ~FacePath()
    {
      *myOs << INDENT << "(" << PATH << ".CLOSEPOLY, (0, 0))";
      *myOs << "]\n";
      *myOs << "face_codes, face_verts = zip(*face_data)\n";
      *myOs << "face_path = " << PATH << "(face_verts, face_codes)\n";
      *myOs << "face_patch = " << PATH_PATCH << "(face_path";
      if(!myColour.empty())
        *myOs << ", facecolor=" << myColour;
      *myOs << ", alpha=0.5";
      *myOs << ")\n";
      *myOs << PLOT << ".add_patch(face_patch)\n";
    }

    void
    moveTo(const Point & p)
    {
      *myOs << INDENT << "(" << PATH << ".MOVETO, " << point(p) << "),\n";
    }

    void
    lineTo(const Point & p)
    {
      *myOs << INDENT << "(" << PATH << ".LINETO, " << point(p) << "),\n";
    }

    void
    curveTo(const Bezier & bezier)
    {
      for(size_t i = 1; i < 4; ++i)
        *myOs << INDENT << "(" << PATH << ".CURVE4, "
            << point(bezier.control[i]) << "),\n";
    }

  private:
    std::string
    point(const Point & p)
    {
      std::stringstream ss;
      ss << "(" << CGAL::to_double(p.x()) << ", " << CGAL::to_double(p.y())
          << ")";
      return ss.str();
    }

    std::ostream * const myOs;
    const std::string myColour;
  };

template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::PATH = "mpath.Path";
template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::PATH_PATCH =
      "mpatches.PathPatch";
template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::INDENT = "    ";
template< typename MapTraits>
  const std::string MatplotlibMapOutputter< MapTraits>::PLOT = "ax";

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::outputArrangement(
      const Arrangement & map) const
  {
    std::string outFile = "map.py";
    if(!mySeedName.empty())
      outFile = mySeedName + "_map.py";
    std::ofstream os(outFile.c_str());
    if(os.is_open())
    {
      outputArrangement(map, &os);
      os.close();
    }
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::outputArrangement(const Arrangement & map,
      std::ostream * const os) const
  {
    out(map, os);
  }

template< typename MapTraits>
  bool
  MatplotlibMapOutputter< MapTraits>::setSeedName(const std::string & seedName)
  {
    mySeedName = seedName;
  }

template< typename MapTraits>
  bool
  MatplotlibMapOutputter< MapTraits>::setColourMap(
      const std::map< Label, int> & colourMap)
  {
    myColourMap = colourMap;
  }

template< typename MapTraits>
  bool
  MatplotlibMapOutputter< MapTraits>::setXLabel(const std::string & label)
  {
    myXLabel = label;
  }

template< typename MapTraits>
  bool
  MatplotlibMapOutputter< MapTraits>::setYLabel(const std::string & label)
  {
    myYLabel = label;
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::out(const Arrangement & map,
      std::ostream * const os) const
  {
    const boost::iterator_range< typename Arrangement::Face_const_iterator> faces(
        map.faces_begin(), map.faces_end());

    *os << "import matplotlib.path as mpath\n"
        << "import matplotlib.patches as mpatches\n"
        << "import matplotlib.pyplot as plt\n\n"
        << "fig, ax = plt.subplots()\n\n";

    const ColourMap & colourMap = getColourMap(faces);
    printProperties(colourMap, os);

    std::string colour, labelName;
    BOOST_FOREACH(const Face & face, faces)
    {
      colour = "";
      labelName = "";

      // Try to find the colour we should used for this face based on the label
      typename ColourMap::const_iterator it;
      if(face.data().label
          && colourMap.find(*face.data().label) != colourMap.end())
      {
        std::stringstream ss;
        ss << "labels['" << *face.data().label << "']['colour']";
        colour = ss.str();
      }

      if(face.data().label)
      {
        std::stringstream ss;
        ss << *face.data().label;
        labelName = ss.str();
      }

      drawFace(face, os, labelName, colour);
    }

    *os << "\nax.autoscale_view()\n";
    *os << "ax.set_aspect(aspect='equal')\n";
    if(!myXLabel.empty())
      *os << "ax.set_xlabel('" << myXLabel << "')\n";
    if(!myYLabel.empty())
      *os << "ax.set_ylabel('" << myYLabel << "')\n";
    *os << "plt.show()\n";
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::drawFace(const Face & face,
      std::ostream * const os, const std::string & label,
      const std::string & colour) const
  {
    if(!face.is_unbounded())
    {
      // Build up the polygon for this face
      CGAL::Polygon_2< typename MapTraits::Kernel> poly;

      { // Draw the path, keep in this scope as FacePath uses destructor to finish up
        FacePath path(os, colour);

        const typename Arrangement::Ccb_halfedge_const_circulator first =
            face.outer_ccb();

        // Set the start point
        if(first->twin()->data().bezier)
          poly.push_back(first->target()->point());
        else
          poly.push_back(first->source()->point());
        path.moveTo(poly.container().back());

        typename Arrangement::Ccb_halfedge_const_circulator cl = first;
        do
        {
          if(cl->data().bezier)
            path.curveTo(*cl->data().bezier);
          else if(cl->twin()->data().bezier)
          {
            Bezier bezier = *cl->twin()->data().bezier;
            bezier.reverse();
            path.curveTo(bezier);
          }
          else
            path.lineTo(cl->target()->point());

          poly.push_back(cl->target()->point());

          ++cl;
        } while(cl != first);
      }

      if(!label.empty())
      {
        drawText(CGAL::centroid(poly.vertices_begin(), poly.vertices_end()),
            label, os);
      }
    }
  }

template< typename MapTraits>
  typename MatplotlibMapOutputter< MapTraits>::ColourMap
  MatplotlibMapOutputter< MapTraits>::getColourMap(
      const boost::iterator_range< typename Arrangement::Face_const_iterator> & faces) const
  {
    // Use algorithm described here:
    // http://stackoverflow.com/questions/43044/algorithm-to-randomly-generate-an-aesthetically-pleasing-color-palette
    // to randomly generate 'pleasing' colours.
    static const int MIX_RED = 255;
    static const int MIX_GREEN = 255;
    static const int MIX_BLUE = 255;

    ColourMap colourMap = myColourMap;

    // Autogenerate colours for the labels that the user hasn't specified
    BOOST_FOREACH(const Face & face, faces)
    {
      const boost::optional< Label> & label = face.data().label;
      if(label)
      {
        if(colourMap.find(*label) == colourMap.end())
        {
          // Not seen this label before, so make up a colour
          const int red = (math::randu< int>(256) + MIX_RED) / 2;
          const int green = (math::randu< int>(256) + MIX_GREEN) / 2;
          const int blue = (math::randu< int>(256) + MIX_BLUE) / 2;

          colourMap[*label] = rgbColour(red, green, blue);
        }
      }
    }
    return colourMap;
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::printProperties(
      const ColourMap & colourMap, std::ostream * const os) const
  {
    SSLIB_ASSERT(os);

    if(colourMap.empty())
      return;

    *os << "labels = dict()\n";
    BOOST_FOREACH(typename ColourMap::const_reference c, colourMap)
    {
      *os << "labels['" << c.first << "'] = {";
      *os << "'colour': '" << toHexString(c.second) << "'";
      *os << "}\n";
    }
    *os << "\n";
  }

template< typename MapTraits>
  std::string
  MatplotlibMapOutputter< MapTraits>::toHexString(const int colour) const
  {
    const int cappedColour = std::max(std::min(colour, 0xffffff), 0);
    // Convert the code to hex string representation
    std::stringstream ss;
    ss << "#";
    ss << std::hex // Print in hexadecimal
        << std::noshowbase // Don't show 0x in front of number
        << std::setfill('0') // Fill with leading 0's if necessary
        << std::internal // Use the internal width (i.e. after 0x)
        << std::setw(6) // Make the internal width 6 numbers
        << cappedColour;
    return ss.str();
  }

template< typename MapTraits>
  void
  MatplotlibMapOutputter< MapTraits>::drawText(const Point pt,
      const std::string & label, std::ostream * const os) const
  {
    using std::string;
    using boost::lexical_cast;

    *os << "ax.text(";

    std::vector< string> params;
    params.push_back(lexical_cast< string>(pt.x()));
    params.push_back(lexical_cast< string>(pt.y()));
    params.push_back("'" + label + "'");
    params.push_back("horizontalalignment='center'");
    params.push_back("verticalalignment='center'");
    *os << boost::algorithm::join(params, ", ") << ")\n";
  }

template< typename MapTraits>
  int
  MatplotlibMapOutputter< MapTraits>::rgbColour(const int red, const int green,
      const int blue) const
  {
    // Clip the colours
    const int r = std::min(std::max(red, 0), 255);
    const int g = std::min(std::max(green, 0), 255);
    const int b = std::min(std::max(blue, 0), 255);
    return (r << 16) + (g << 8) + b;
  }

}
}

#endif /* MATPLOTLIB_MAP_OUTPUTTER_DETAIL_H */
