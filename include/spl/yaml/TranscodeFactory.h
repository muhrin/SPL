/*
 * TranscodeFactory.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef TRANSCODE_FACTORY_H
#define TRANSCODE_FACTORY_H

// INCLUDES //////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_YAML

#include <yaml-cpp/yaml.h>

#include "spl/factory/FactoryFwd.h"
#include "spl/yaml/HelperTypes.h"

// NAMESPACES ////////////////////////////////

// Some custom YAML transcoders
namespace YAML {

template< >
  struct convert< spl::factory::AtomSpeciesCount> : public spl::yaml::detail::ConvertStreamableScalar<
      spl::factory::AtomSpeciesCount>
  {
    typedef spl::yaml::detail::ConvertStreamableScalar< spl::factory::AtomSpeciesCount> Base;
    using Base::encode;
    using Base::decode;
  };

}

#endif /* SPL_USE_YAML */
#endif /* TRANSCODE_FACTORY_H */
