/*
 * SsLibElements.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SsLibElements.h"


#include "factory/SchemaAnon.h"
#include "factory/SchemaList.h"
#include "factory/SchemaMap.h"
#include "factory/SchemaMapSingle.h"
#include "factory/SchemaScalarValueLiteral.h"
#include "factory/SchemaObjectUid.h"

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace factory {

SsLibElements::SsLibElements():
// Num
n(new SchemaMapSingle("n", true, "sslib.n")),

// Atom description
atomDesc(new SchemaMap("atom", true, "sslib.atomDesc")),
atomSpec(new SchemaMapSingle("spec", true, "sslib.atomDesc.spec")),
atomsDescList(new SchemaList("atoms", true, "sslib.atomsDescList")),

// Structure description
strDesc(new SchemaMap("strDesc", true, "sslib.strDesc")),

// Cell description
cellDesc(new SchemaMap("cellDesc", true, "sslib.cellDesc")),
cellDescVol(new SchemaMapSingle("vol", true, "sslib.cellDesc.bol")),
cellDescParams(new SchemaList("params", true, "sslib.cellDesc.params", 6)),

// Structure constraints
strDescConstraintsList(new SchemaList("constraints", false, "sslib.strDesc.constraints")),
strDescConstraintTyp(new SchemaMap("constraintTyp", true, "sslib.strDesc.constraintTyp", true)),

// Structure generator
strGenerator(new SchemaMap("strGenerator", true, "sslib.strGenerator")),
strGeneratorTyp(new SchemaMap("strGeneratorTyp", false, "sslib.strGeneratorTyp", true)),

// Cell generator
cellGenerator(new SchemaMap("cellGenerator", true, "sslib.cellGenerator")),
cellGeneratorTyp(new SchemaMap("cellGeneratorTyp", false, "sslib.cellGeneratorTyp", true)),

// Potential
potential(new SchemaMap("potential", true, "sslib.potential", false)),
potentialTyp(new SchemaMap("potentialTyp", true, "sslib.potentialTyp", true))
{
  // Atom description
  atomDesc->insert(atomDesc->newInstance());
  atomDesc->insert(n->newInstance());

  // Structure description
  strDesc->insert(atomsDescList->newInstance());

  // Cell description
  cellDesc->insert(cellDescVol->newInstance());
  cellDesc->insert(cellDescParams->newInstance());

  // Structure constraints
  strDescConstraintsList->insert(strDescConstraintTyp->newInstance());

  // Cell generator
  cellGenerator->insert(cellGeneratorTyp->newInstance());
  cellGeneratorTyps._default = MapPtr(new SchemaMap("default", false, "sslib.cellGenerator.default", false, cellGeneratorTyp->getUid()));

  // Structure generator
  strGenerator->insert(strGeneratorTyp->newInstance());
  strGenerator->insert(cellGenerator->newInstance());
  strGeneratorTyps._default = MapPtr(new SchemaMap("default", false, "sslib.strGenerator.default", false, strGeneratorTyp->getUid()));

  // Potential
  potential->insert(potentialTyp->newInstance());

  { // PairPot
    potentialTyps.pairPot = MapPtr(new SchemaMap("pairPot", true, "sslib.potential.pairPot", false, potentialTyp->getUid()));
    potentialTyps.pairPot->insertAnon(ListPtr(new SchemaList("e", true, SchemaElement::UID_ANONYMOUS)));
    potentialTyps.pairPot->insertAnon(ListPtr(new SchemaList("s", true, SchemaElement::UID_ANONYMOUS)));
    potentialTyps.pairPot->insertAnon(ListPtr(new SchemaList("b", true, SchemaElement::UID_ANONYMOUS)));
    potentialTyps.pairPot->insertAnon(ScalarPtr(new SchemaMapSingle("cut", true, SchemaElement::UID_ANONYMOUS)));
    ScalarPtr comb(new SchemaMapSingle("comb", true, SchemaElement::UID_ANONYMOUS));
    comb->insert("none");
    comb->insert("uhrin");
    comb->insert("lorentz");
    comb->insert("berthelot");
    comb->insert("lorentzBerthelot");
    potentialTyps.pairPot->insertAnon(comb);

  }
}

}
}


