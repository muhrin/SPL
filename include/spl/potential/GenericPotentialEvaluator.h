/*
 * GenericPotentialEvaluator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef GENERIC_POTENTIAL_EVALUATOR_H
#define GENERIC_POTENTIAL_EVALUATOR_H

// INCLUDES /////////////////////////////////////////////
#include <memory>

#include "spl/potential/IPotential.h"
#include "spl/potential/IPotentialEvaluator.h"

namespace spl {
namespace potential {


// FORWARD DECLARATIONS ////////////////////////////////////
class IPotential;
struct PotentialData;
class Structure;

template <class Potential>
class GenericPotentialEvaluator : public IPotentialEvaluator
{
public:

  typedef typename Potential::DataType DataTyp;
  typedef IPotentialEvaluator::EvalResult EvalResult;

  GenericPotentialEvaluator(
    const Potential & potential,
    const common::Structure & structure,
    ::std::auto_ptr<DataTyp> & data);

  // From IPotentialEvaluator
  virtual PotentialData & getData();

  virtual EvalResult evalPotential();

  virtual const IPotential & getPotential() const;
  // End from IPotentialEvaluator

private:

  const common::Structure &     myStructure;
  const Potential &             myPotential;
  ::std::auto_ptr<DataTyp>      myData;

};

// IMPLEMENTATION //////////////////

template <class Potential>
GenericPotentialEvaluator<Potential>::GenericPotentialEvaluator(
  const Potential & potential,
  const common::Structure & structure,
  ::std::auto_ptr<DataTyp> & data):
myStructure(structure),
myPotential(potential),
myData(data)
{}

template <class Potential>
PotentialData & GenericPotentialEvaluator<Potential>::getData()
{
  return *myData;
}

template <class Potential>
typename GenericPotentialEvaluator<Potential>::EvalResult
GenericPotentialEvaluator<Potential>::evalPotential()
{
  return EvalResult(myData.get(), myPotential.evaluate(myStructure, *myData));
}

template <class Potential>
const IPotential & GenericPotentialEvaluator<Potential>::getPotential() const
{
  return myPotential;
}

}
}

#endif /* GENERIC_POTENTIAL_EVALUATOR_H */
