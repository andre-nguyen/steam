//////////////////////////////////////////////////////////////////////////////////////////////
/// \file CostTermCollectionBase.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_COST_TERM_COLLECTION_BASE_HPP
#define STEAM_COST_TERM_COLLECTION_BASE_HPP

#include <memory>

#include <steam/problem/CostTerm.hpp>

#include <steam/blockmat/BlockSparseMatrix.hpp>
#include <steam/blockmat/BlockVector.hpp>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Class that fully defines a nonlinear cost term (or 'factor').
///        Cost terms are composed of an error function, loss function and noise model.
//////////////////////////////////////////////////////////////////////////////////////////////
class CostTermCollectionBase
{
 public:

  /// Convenience typedefs
  typedef std::shared_ptr<CostTermCollectionBase> Ptr;
  typedef std::shared_ptr<const CostTermCollectionBase> ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  CostTermCollectionBase() {}

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Compute the cost from the collection of cost terms
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual double cost() const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get size of the collection
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual size_t size() const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Build the left-hand and right-hand sides of the Gauss-Newton system of equations
  ///        using the cost terms in this collection.
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void buildGaussNewtonTerms(const StateVector& stateVector,
                                     BlockSparseMatrix* approximateHessian,
                                     BlockVector* gradientVector) const = 0;
};

} // steam

#endif // STEAM_COST_TERM_COLLECTION_BASE_HPP
