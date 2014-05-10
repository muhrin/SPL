/*
 * GenerationSettings.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef GENERATION_SETTINGS_H
#define GENERATION_SETTINGS_H

// INCLUDES /////////////////////////////////
#include <map>

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {

struct AtomSettings
{
  size_t num;
};

struct GenerationSettings
{
  typedef ::std::map< ::std::string, AtomSettings> AtomsSettings;
  AtomsSettings atomsSettings;
};

}
}

#endif /* GENERATION_SETTINGS_H */
