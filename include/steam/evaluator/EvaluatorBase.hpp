//////////////////////////////////////////////////////////////////////////////////////////////
/// \file EvaluatorBase.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_EVALUATOR_BASE_HPP
#define STEAM_EVALUATOR_BASE_HPP

#include <Eigen/Core>

#include <steam/StateVector.hpp>
#include <steam/evaluator/jacobian/JacobianTreeNode.hpp>
#include <steam/evaluator/jacobian/EvalTreeNode.hpp>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Base class that defines the general 'evaluator' interface
//////////////////////////////////////////////////////////////////////////////////////////////
template <typename EvalType>
class EvaluatorBase
{
 public:

  /// Convenience typedefs
  typedef boost::shared_ptr<EvaluatorBase<EvalType> > Ptr;
  typedef boost::shared_ptr<const EvaluatorBase<EvalType> > ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Default constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  EvaluatorBase() {}

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Returns whether or not an evaluator contains unlocked state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool isActive() const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface for the general 'evaluation'
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual EvalType evaluate() const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface for the general 'evaluation', with Jacobians
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual EvalType evaluate(std::vector<Jacobian>* jacs) const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface for the general 'evaluation', with Jacobian tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual std::pair<EvalType, JacobianTreeNode::ConstPtr> evaluateJacobians() const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface for the general 'evaluation', with Jacobian tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual EvalTreeNode<EvalType>* evaluateTree() const = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface for the general evaluation of the Jacobian tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void appendJacobians(const Eigen::MatrixXd& lhs,
                               EvalTreeNode<EvalType>* evaluationTree,
                               std::vector<Jacobian>* outJacobians) const = 0;
  //if (isActive()) {appendJacobians(lhs*f, outJacobians);}


};

} // steam

#endif // STEAM_EVALUATOR_BASE_HPP
