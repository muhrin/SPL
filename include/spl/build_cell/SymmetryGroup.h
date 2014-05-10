/*
 * SymmetryGroup.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SYMMETRY_GROUP_H
#define SYMMETRY_GROUP_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <map>
#include <set>
#include <vector>

#include <armadillo>

// DEFINITION ///////////////////////

namespace spl {
namespace build_cell {

struct EigenvectorsData;

class SymmetryGroup
{
public:
  typedef ::arma::mat44 SymOp;
  typedef ::arma::mat Eigenspace;
  typedef ::std::vector<Eigenspace> Eigenspaces;
  typedef ::std::pair<double, Eigenspace> EigenvalAndEigenspace;
  typedef ::std::vector<EigenvalAndEigenspace> EigenvalsAndEigenspaces;
  typedef ::std::vector<unsigned int> Multiplicities;
  typedef ::std::vector<bool> OpMask;
  typedef ::std::pair<Eigenspace, OpMask> EigenspaceAndMask;
  typedef ::std::vector<EigenspaceAndMask> EigenspacesAndMasks;
private:
  typedef ::std::vector<SymOp> SymOps;
public:
  typedef SymOps::const_iterator OperatorsIterator;

  SymmetryGroup();

  void reset();
  void generateEigenvectors();

  void addOp(const SymOp & symmetryOp);

  size_t numOps() const;
  const ::arma::mat44 & getOp(const size_t idx) const;
  OperatorsIterator beginOperators() const;
  OperatorsIterator endOperators() const;
  bool isInGroup(const SymOp & op, const double tolerance = 1e-10);
  
  Multiplicities getMultiplicities() const;

  const EigenspacesAndMasks * getEigenspacesAndMasks(const unsigned int multiplicity) const;

protected:
  typedef ::std::map<unsigned int, EigenspacesAndMasks> InvariantsMap;

  void generateMultiplicityEigenvectors2();
  bool getEigenspaces(
    EigenvalsAndEigenspaces & spaces,
    const ::arma::cx_vec & eigenvals,
    const ::arma::cx_mat & eigenvecs
  ) const;
  void insertEigenspace(
    ::std::vector<EigenvectorsData> & eigenvectorsData,
    const size_t operatorIndex,
    const double eigenvalue,
    const Eigenspace & eigenspace
  ) const;
  size_t findCompatibleEigenspaces(
    ::std::vector<size_t> compatibleEigenspaces,
    const ::std::vector<EigenvectorsData> & eigenvectorsData,
    const Eigenspace & eigenspace
  ) const;
  ::std::set<double>::const_iterator findEigenvalue(
    const ::std::set<double> & eigenvalues,
    const double eigenvalue
  ) const;


  void getReal(::arma::mat & realSpace, const ::arma::cx_mat & space) const;
  bool addToSpace(::arma::mat & space, const ::arma::vec & basisVec) const;

  bool isReal(const ::std::complex<double> & value) const;
  
  SymOps mySymOps;
  InvariantsMap myInvariantsMap;
};

}
}


#endif /* SYMMETRY_GROUP_H */
