/*
 * SsLibElements.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_ELEMENTS_H
#define SSLIB_ELEMENTS_H

// INCLUDES /////////////////////////////////////////////

#include <boost/shared_ptr.hpp>

// DEFINES //////////////////////////////////////////////

namespace sstbx {
namespace factory {

class SchemaList;
class SchemaMap;
class SchemaMapSingle;


// FORWARD DECLARATIONS ////////////////////////////////////

class SsLibElements
{
public:

  typedef ::boost::shared_ptr<SchemaList>     ListPtr;
  typedef ::boost::shared_ptr<SchemaMap>      MapPtr;
  typedef ::boost::shared_ptr<SchemaMapSingle>   ScalarPtr;


  SsLibElements();

  // Generic number of things i.e. +ve integer
  ScalarPtr   n;

  // Atom description
  MapPtr      atomDesc;
  ScalarPtr   atomSpec;
  ListPtr     atomsDescList;

  // Cell description
  MapPtr      cellDesc;
  ListPtr     cellDescParams;
  ScalarPtr   cellDescVol;

  // Structure description
  MapPtr      strDesc;

  // Structure constraints
  ListPtr     strDescConstraintsList;
  MapPtr      strDescConstraintTyp;
  struct strDescConstraintTyps
  {
  } strDescConstraintTyps;

  // Structure generator
  MapPtr      strGenerator;
  MapPtr      strGeneratorTyp;
  struct strGeneratorTyps
  {
    MapPtr  _default;
  } strGeneratorTyps;

  // Cell generator
  MapPtr      cellGenerator;
  MapPtr      cellGeneratorTyp;
  struct cellGeneratorTyps
  {
    MapPtr    _default;
  } cellGeneratorTyps;

  // Potential
  MapPtr      potential;
  MapPtr      potentialTyp;
  struct potentialTyps
  {
    MapPtr pairPot;
  } potentialTyps;


};
}
}

#endif /* SSLIB_ELEMENTS_h */

