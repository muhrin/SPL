/*
 * AtomFormatParser.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/AtomFormatParser.h"

#include "spl/factory/SsLibYamlSchema.h"

// DEFINES /////////////////////////////////

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {

template< >
  template< >
    void
    AtomFormatParser< factory::builder::SimpleAtomsData>::init<
        factory::builder::SimpleAtomsData>()
    {
      addEntry("spec", &factory::builder::SimpleAtomsData::species);
      addEntry("radius", &factory::builder::SimpleAtomsData::radius);
      addEntry("pos", &factory::builder::SimpleAtomsData::pos);
      addEntry("label", &factory::builder::SimpleAtomsData::label);
    }

}
}
