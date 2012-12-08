/*
 * SsLibYamlKeywords.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_YAML_KEYWORDS_H
#define SSLIB_YAML_KEYWORDS_H

// INCLUDES //////////////////////////////////
#include "SSLib.h"

#ifdef SSLIB_USE_YAML

#include <string>

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace factory {
namespace sslib_yaml_keywords {

/** The id type used by elements in the yaml document */
typedef ::std::string     KwTyp;

/**
/* A list of the keywords used in the yaml sslib input files
/**/
// General
static const KwTyp TYPE                  = "type";
static const KwTyp VALUE                 = "value";
static const KwTyp DEFAULT               = "default";
static const KwTyp MIN                   = "min";
static const KwTyp MAX                   = "max";

// Structure description ///
static const KwTyp STRUCTURE                      = "structure";
static const KwTyp STRUCTURE__ATOM_INFO_FORMAT    = "atomInfoFormat";
static const KwTyp STRUCTURE__ATOM_DEFAULTS       = "atomDefaults";
static const KwTyp STRUCTURE__ATOMS               = "atoms";
static const KwTyp STRUCTURE__ATOMS__INFO         = "info";
static const KwTyp STRUCTURE__ATOMS__SPEC         = "spec";
static const KwTyp STRUCTURE__ATOMS__POS          = "pos";
static const KwTyp STRUCTURE__CELL                = "cell";
static const KwTyp STRUCTURE__CELL__ABC           = "abc";
static const KwTyp STRUCTURE__CELL__VOL           = "vol";
static const KwTyp STRUCTURE__NAME                = "name";
static const KwTyp STRUCTURE__PROPERTIES          = "properties";

// Random structure ///////////////
static const KwTyp RANDOM_STRUCTURE                       = "randomStructure";
static const KwTyp RANDOM_STRUCTURE__ATOM_RADII           = "atomRadii";
static const KwTyp RANDOM_STRUCTURE__ATOMS__RADIUS        = "radius";
static const KwTyp RANDOM_STRUCTURE__ATOMS__POS           = STRUCTURE__ATOMS__POS;

static const KwTyp STR_DESC              = "strDesc";
static const KwTyp STR_DESC__ATOMS       = STRUCTURE__ATOMS;
static const KwTyp STR_DESC__ATOMS__SPEC = "spec";
static const KwTyp STR_DESC__ATOMS__N    = "n";

static const KwTyp STR_GENERATOR                    = "strGenerator";
static const KwTyp STR_GENERATOR__TYPE___DEFAULT    = DEFAULT;

// Unit cell ////////////////////////////////
static const KwTyp CELL                       = "cell";
static const KwTyp CELL__PARAMS               = "params";

// Random unit cell /////////////////////////
static const KwTyp RANDOM_CELL                      = "randomCell";
static const KwTyp RANDOM_CELL__PARAMS              = CELL__PARAMS;
static const KwTyp RANDOM_CELL__VOL                 = "vol";
static const KwTyp RANDOM_CELL__VOL_DELTA           = "volDelta";
static const KwTyp RANDOM_CELL__ANGLES              = "angles";
static const KwTyp RANDOM_CELL__ANGLES__MIN         = MIN;
static const KwTyp RANDOM_CELL__ANGLES__MAX         = MAX;
static const KwTyp RANDOM_CELL__LENGTHS             = "lengths";
static const KwTyp RANDOM_CELL__LENGTHS__MIN        = MIN;
static const KwTyp RANDOM_CELL__LENGTHS__MAX        = MAX;
static const KwTyp RANDOM_CELL__LENGTHS__MAX_RATIO  = "maxRatio";

static const KwTyp POTENTIAL                    = "potential";
static const KwTyp POTENTIAL__TYPE___PAIR_POT   = "pairPot";
static const KwTyp POTENTIAL__PAIR_POT__E       = "e";
static const KwTyp POTENTIAL__PAIR_POT__S       = "s";
static const KwTyp POTENTIAL__PAIR_POT__CUT     = "cut";
static const KwTyp POTENTIAL__PAIR_POT__B       = "b";
static const KwTyp POTENTIAL__PAIR_POT__POW     = "pow";
static const KwTyp POTENTIAL__PAIR_POT__COMB    = "comb";

static const KwTyp OPTIMISER__TYPE___TPSD       = "tpsd";

static const KwTyp STR_SET                = "strSet";

static const KwTyp STR_COMPARATOR                     = "strComparator";
static const KwTyp STR_COMPARATOR__TYPE___SORTED_DIST = "sortedDist";

static const KwTyp STR_WRITER             = "strWriter";
static const KwTyp STR_WRITER__TYPE___RES = "res";


}
}
}

#endif /* SSLIB_YAML_KEYWORDS_H */

#endif /* SSLIB_USE_YAML */
