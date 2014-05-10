/*
 * InfoLine.h
 *
 *
 *  Created on: Feb 7, 2014
 *      Author: Martin Uhrin
 */

#ifndef INFO_LINE_H
#define INFO_LINE_H

// INCLUDES /////////////////////////////////////////////
#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

// DEFINES ////////////////////////////////////////

// FUNCTIONS ////////////////////////////////////////

namespace spl {
// FORWARD DECLARES ////////////////////////////////
namespace common {
class Structure;
}

namespace io {

struct InfoLine
{
  InfoLine();
  InfoLine(const common::Structure & structure);
  boost::optional< std::string> name;
  boost::optional< double> pressure;
  boost::optional< double> volume;
  boost::optional< double> enthalpy;
  boost::optional< double> spinDensity;
  boost::optional< double> integratedSpinDensity;
  boost::optional< size_t> numAtoms;
  boost::optional< std::string> spaceGroup;
  boost::optional< unsigned int> timesFound;

  void
  set(const common::Structure & structure);
  void
  populate(common::Structure * const structure) const;
};

::std::ostream &
operator <<(std::ostream & os, const InfoLine & line);

std::istream &
operator >>(std::istream &in, InfoLine & line);

namespace detail {
extern const std::string EMPTY;

template< typename T>
  void
  setValue(const std::string str, boost::optional< T> * const member)
  {
    if(str.empty() || str == EMPTY)
      return;

    try
    {
      *member = boost::lexical_cast< T>(str);
    }
    catch(const boost::bad_lexical_cast & /*e*/)
    {
    }
  }

}
}
}

#endif /* INFO_LINE_H */
