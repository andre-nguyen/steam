//////////////////////////////////////////////////////////////////////////////////////////////
/// \file VanillaGaussNewtonSolver.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_VANILLA_GAUSS_NEWTON_SOLVER_HPP
#define STEAM_VANILLA_GAUSS_NEWTON_SOLVER_HPP

#include <Eigen/Core>
#include <memory>

#include <steam/solver/GaussNewtonSolverBase.hpp>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief A vanilla Gauss-Newton solver (no line search or trust region method)
//////////////////////////////////////////////////////////////////////////////////////////////
class VanillaGaussNewtonSolver : public GaussNewtonSolverBase
{
public:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Parameter type is the basic
  //////////////////////////////////////////////////////////////////////////////////////////////
  typedef SolverBase::Params Params;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  VanillaGaussNewtonSolver(OptimizationProblem* problem, const Params& params = Params());

private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Build the system, solve for a step size and direction, and update the state
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool linearizeSolveAndUpdate(double* newCost);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Casts parameters to base type (for SolverBase class)
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual const SolverBase::Params& getSolverBaseParams() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Parameters
  //////////////////////////////////////////////////////////////////////////////////////////////
  Params params_;

};

} // steam

#endif // STEAM_VANILLA_GAUSS_NEWTON_SOLVER_HPP
