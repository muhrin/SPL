/*
 * SymmetryGroup.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/SymmetryGroup.h"

#include <iostream>
#include <complex>
#include <vector>

#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"
#include "spl/math/Matrix.h"
#include "spl/utility/IndexingEnums.h"
#include "spl/utility/StableComparison.h"

namespace spl {
namespace build_cell {

namespace comp = utility::stable;

struct EigenvectorsData
{
  typedef ::std::vector<bool> OpMask;
  typedef ::std::set<double> Eigenvalues;

  SymmetryGroup::Eigenspace eigenspace;
  Eigenvalues eigenvalues;
  OpMask opMask;
  unsigned int multiplicity;
};

SymmetryGroup::SymmetryGroup()
{
  // Every symmetry group has the identity operator
  mySymOps.push_back(::arma::eye< ::arma::mat>(4, 4));
}

void SymmetryGroup::reset()
{
  mySymOps.clear();
  myInvariantsMap.clear();
}

void SymmetryGroup::addOp(const ::arma::mat44 & symmetryOp)
{
  mySymOps.push_back(symmetryOp);
}

void SymmetryGroup::generateEigenvectors()
{
  generateMultiplicityEigenvectors2();
}

size_t SymmetryGroup::numOps() const
{
  return mySymOps.size();
}

const ::arma::mat44 & SymmetryGroup::getOp(const size_t idx) const
{
  SSLIB_ASSERT(idx < mySymOps.size());
  return mySymOps[idx];
}

SymmetryGroup::OperatorsIterator
SymmetryGroup::beginOperators() const
{
  return mySymOps.begin();
}

SymmetryGroup::OperatorsIterator
SymmetryGroup::endOperators() const
{
  return mySymOps.end();
}

bool SymmetryGroup::isInGroup(const SymOp & op, const double tolerance)
{
  BOOST_FOREACH(const SymOp & myOp, mySymOps)
  {
    if(math::equals(myOp, op, tolerance))
      return true;
  }
  return false;
}

SymmetryGroup::Multiplicities
SymmetryGroup::getMultiplicities() const
{
  Multiplicities mults;
  mults.reserve(myInvariantsMap.size());
  BOOST_FOREACH(InvariantsMap::const_reference entry, myInvariantsMap)
  {
    mults.push_back(entry.first);
  }
  return mults;
}

const SymmetryGroup::EigenspacesAndMasks *
SymmetryGroup::getEigenspacesAndMasks(const unsigned int multiplicity) const
{
  const InvariantsMap::const_iterator it = myInvariantsMap.find(multiplicity);
  if(it == myInvariantsMap.end())
    return NULL;

  return &(it->second);
}

void SymmetryGroup::generateMultiplicityEigenvectors2()
{
  myInvariantsMap.clear();

  ::std::vector<EigenvectorsData> invariantsData;

  ::arma::cx_vec eigval;
  ::arma::cx_mat eigvec;

  EigenvalsAndEigenspaces spaces;
  // NOTE: Always skip over first (identity) operator
  for(size_t op = 1; op < numOps(); ++op)
  {
    if(eig_gen(eigval, eigvec, mySymOps[op].submat(0, 0, 2, 2)))
    {
      ::std::cout << mySymOps[op] << "\n";
      ::std::cout << eigval << "\n" << eigvec << "\n";

      ::arma::mat U, V;
      ::arma::vec s;
      svd(U,s,V,mySymOps[op].submat(0, 0, 2, 2));
      std::cout << "== SVD ==" << "\nU:\n" << U << "\ns:\n" << s << "\nV:\n" << V << "\n";


      getEigenspaces(spaces, eigval, eigvec);
      // Insert all the invariants
      BOOST_FOREACH(const EigenvalAndEigenspace & space, spaces)
      {
        insertEigenspace(invariantsData, op, space.first, space.second);
      }
    }
  }

  // Now that we have figured out the invariants and multiplicities, save them
  BOOST_FOREACH(const EigenvectorsData & invariantData, invariantsData)
  {
    if(invariantData.multiplicity != mySymOps.size())
    {
      // Add a new invariant to the list
      EigenspacesAndMasks & invariantAndMasks = myInvariantsMap[invariantData.multiplicity];
      invariantAndMasks.resize(invariantAndMasks.size() + 1);

      // Save the invariant itself and the opmask, leave the rest
      EigenspaceAndMask & invariantAndMask = invariantAndMasks.back();
      invariantAndMask.first = invariantData.eigenspace;
      invariantAndMask.second = invariantData.opMask;
    }
  }
}

bool SymmetryGroup::getEigenspaces(
  EigenvalsAndEigenspaces & spaces,
  const ::arma::cx_vec & eigenvals,
  const ::arma::cx_mat & eigenvecs
) const
{
  spaces.clear();

  const size_t numEigens = eigenvals.n_rows;

  // Look for degenerate eigenvalues
  ::std::vector<bool> dealtWith(numEigens, false);

  ::std::vector<size_t> sameEigenvals;
  double eigenval;
  for(size_t i = 0; i < numEigens; ++i)
  {
    if(!dealtWith[i] && isReal(eigenvals(i)))
    {
      eigenval = ::std::real(eigenvals(i));
      sameEigenvals.clear();
      sameEigenvals.push_back(i);
      for(size_t j = i + 1; j < numEigens; ++j)
      {
        if(!dealtWith[j] && isReal(eigenvals(j)) && comp::eq(::std::real(eigenvals(j)), eigenval))
        {
          sameEigenvals.push_back(j);
          dealtWith[j] = true;
        }
      }

      // Turn the list of indices into a matrix representing the eigenspace
      ::arma::cx_mat space(eigenvecs.n_rows, sameEigenvals.size());
      for(size_t j = 0; j < sameEigenvals.size(); ++j)
        space.col(j) = eigenvecs.col(sameEigenvals[j]);

      ::arma::mat realSpace;
      getReal(realSpace, space);

      spaces.push_back(::std::make_pair(eigenval, realSpace));
    }
    dealtWith[i] = true;
  }

  return !spaces.empty(); // Did we find any?
}

void SymmetryGroup::insertEigenspace(
  ::std::vector<EigenvectorsData> & eigenvectorsData,
  const size_t operatorIndex,
  const double eigenvalue,
  const Eigenspace & eigenspace) const
{
  ::std::vector<size_t> compatibleEigenspaces;
  const size_t coincidentEigenspace =
    findCompatibleEigenspaces(compatibleEigenspaces, eigenvectorsData, eigenspace);

  // Special positions are defined as those with an eigenvalue of 1
  // i.e. those that do not change the point from its position
  const bool isSpecialPos = comp::eq(eigenvalue, 1.0);
  
  // Is this eigenspace coincident with one we have already seen?
  if(coincidentEigenspace != eigenvectorsData.size())
  {
    // Have we had this eigenvalue and eigenspace combination?
    const ::std::set<double>::const_iterator eigValIt =
      findEigenvalue(eigenvectorsData[coincidentEigenspace].eigenvalues, eigenvalue);
    if(eigValIt != eigenvectorsData[coincidentEigenspace].eigenvalues.end() || isSpecialPos)
    {
      eigenvectorsData[coincidentEigenspace].opMask[operatorIndex] = false;
      --eigenvectorsData[coincidentEigenspace].multiplicity;
    }
    else
      eigenvectorsData[coincidentEigenspace].eigenvalues.insert(eigenvalue);
  }
  else
  {
    // Never seen this eigenspace before so insert it
    const size_t groupOrder = mySymOps.size();
    eigenvectorsData.resize(eigenvectorsData.size() + 1);

    EigenvectorsData & newSpaceData = eigenvectorsData.back();

    newSpaceData.eigenvalues.insert(eigenvalue);
    newSpaceData.eigenspace = eigenspace;
    // Mask out this operator if the eigenvectors correspond to special positions
    newSpaceData.opMask.resize(groupOrder, true);
    newSpaceData.opMask[operatorIndex] = !isSpecialPos;
    // Eigenvalues of 1 mean no change to point so multiplicity is one
    // less than the order of the group
    newSpaceData.multiplicity = isSpecialPos ? groupOrder - 1 : groupOrder;

    // Now we have to go through and deal with all the spanning and spanned eigenspaces
    BOOST_FOREACH(const size_t idx, compatibleEigenspaces)
    {
      if(eigenvectorsData[idx].eigenspace.n_cols > eigenspace.n_cols)
      { // We are spanned by this space
        if(findEigenvalue(eigenvectorsData[idx].eigenvalues, eigenvalue) != 
          eigenvectorsData[idx].eigenvalues.end())
        { // This space has an operator with the same eigenvalue
          --newSpaceData.multiplicity;
          // Do bitwise and with the opmask of the other eigenspace
          for(size_t i = 0; i < newSpaceData.opMask.size(); ++i)
            newSpaceData.opMask[i] = (newSpaceData.opMask[i] && eigenvectorsData[idx].opMask[i]);
        }
        else
        {
          newSpaceData.eigenvalues.insert(
            eigenvectorsData[idx].eigenvalues.begin(),
            eigenvectorsData[idx].eigenvalues.end()
          );
        }
      }
      else
      { // We span this space
        if(findEigenvalue(eigenvectorsData[idx].eigenvalues, eigenvalue) != 
          eigenvectorsData[idx].eigenvalues.end())
        { // This space has an operator with the same eigenvalue
          --eigenvectorsData[idx].multiplicity;
          eigenvectorsData[idx].opMask[idx] = false;
        }
        else
        {
          eigenvectorsData[idx].eigenvalues.insert(eigenvalue);
        }
      }
    }
  }
}

size_t SymmetryGroup::findCompatibleEigenspaces(
  ::std::vector<size_t> compatibleEigenspaces,
  const ::std::vector<EigenvectorsData> & eigenvectorsData,
  const Eigenspace & eigenspace
) const
{
  compatibleEigenspaces.clear();
  size_t coincidentSpace = eigenvectorsData.size();
  for(size_t i = 0; i < eigenvectorsData.size(); ++i)
  {
    // Which space has a higher dimensionality?
    if(eigenvectorsData[i].eigenspace.n_cols == eigenspace.n_cols)
    {
      // The two spaces are of the same dimensionality, if they 'span' eachother
      // then they are coincident
      if(math::isSpannedBy(eigenvectorsData[i].eigenspace, eigenspace))
        coincidentSpace = i;
    }
    else if(eigenvectorsData[i].eigenspace.n_cols >= eigenspace.n_cols)
    {
      if(math::isSpannedBy(eigenvectorsData[i].eigenspace, eigenspace))
        compatibleEigenspaces.push_back(i);
    }
    else if(eigenvectorsData[i].eigenspace.n_cols < eigenspace.n_cols)
    {
      if(math::isSpannedBy(eigenspace, eigenvectorsData[i].eigenspace))
        compatibleEigenspaces.push_back(i);
    }
  }
  return coincidentSpace;
}

::std::set<double>::const_iterator SymmetryGroup::findEigenvalue(
  const ::std::set<double> & eigenvalues,
  const double eigenvalue
) const
{
  ::std::set<double>::const_iterator it = eigenvalues.begin();
  for(const ::std::set<double>::const_iterator end = eigenvalues.end();
    it != end; ++it)
  {
    if(utility::stable::eq(*it, eigenvalue))
      break;
  }
  return it;
}

void SymmetryGroup::getReal(::arma::mat & realSpace, const ::arma::cx_mat & space) const
{
  ::arma::mat subspace;
  ::arma::vec vec;
  for(size_t i = 0; i < space.n_cols; ++i)
  {
    if(!math::isZero(::arma::real(space.col(i))))
      addToSpace(subspace, ::arma::real(space.col(i)));

    if(!math::isZero(::arma::imag(space.col(i))))
      addToSpace(subspace, ::arma::imag(space.col(i)));
  }
  // TODO: Now orthonormalise the real space
  
  // TODO: Have to rotate the whole space s.t. it moves into the real plane
  // if possible
  ::arma::mat Q, R;
  ::arma::qr(Q, R, subspace);
  std::cout << "== QR ==" << "\nA:\n" << subspace << "\nQ:\n" << Q << "\nR:\n" << R << "\n";

  realSpace = subspace;
}

bool SymmetryGroup::addToSpace(::arma::mat & space, const ::arma::vec & basisVec) const
{
  if(space.empty())
  {
    space.resize(basisVec.n_rows, 1);
    space.col(0) = basisVec;
    return true;
  }
  else
  {
    // Does the space already span this basis vector?
    if(!math::isSpannedBy(space, basisVec))
    {
      const size_t numCols = space.n_cols;
      space.resize(space.n_rows, numCols + 1);
      space.col(numCols) = basisVec;
      return true;
    }
  }
  return false;
}

bool SymmetryGroup::isReal(const ::std::complex<double> & value) const
{
  return comp::eq(std::imag(value), 0.0);
}


}
}
