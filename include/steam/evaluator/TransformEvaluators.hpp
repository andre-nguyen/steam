//////////////////////////////////////////////////////////////////////////////////////////////
/// \file TransformEvaluators.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_TRANSFORM_EVALUATORS_HPP
#define STEAM_TRANSFORM_EVALUATORS_HPP

#include <Eigen/Core>

#include <steam/evaluator/EvaluatorBase.hpp>
#include <steam/state/LieGroupStateVar.hpp>

namespace steam {
namespace se3 {

// Basic types

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluator for a transformation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
typedef EvaluatorBase<lgmath::se3::Transformation> TransformEvaluator;

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluator for a 6D vector
//////////////////////////////////////////////////////////////////////////////////////////////
typedef EvaluatorBase<Eigen::Matrix<double,6,1> > Vector6dEvaluator;

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluator for a 4D vector
//////////////////////////////////////////////////////////////////////////////////////////////
typedef EvaluatorBase<Eigen::Vector4d> Vector4dEvaluator;

// Transformation evaluators

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Simple transform evaluator for a transformation state variable
//////////////////////////////////////////////////////////////////////////////////////////////
class TransformStateEvaluator : public TransformEvaluator
{
 public:

  /// Convenience typedefs
  typedef boost::shared_ptr<TransformStateEvaluator> Ptr;
  typedef boost::shared_ptr<const TransformStateEvaluator> ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  TransformStateEvaluator(const se3::TransformStateVar::ConstPtr& transform);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Pseudo constructor - return a shared pointer to a new instance
  //////////////////////////////////////////////////////////////////////////////////////////////
  static Ptr MakeShared(const se3::TransformStateVar::ConstPtr& transform);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Returns whether or not an evaluator contains unlocked state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool isActive() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual lgmath::se3::Transformation evaluate() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix and Jacobian (identity)
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual lgmath::se3::Transformation evaluate(std::vector<Jacobian>* jacs) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix and Jacobian (identity)
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual std::pair<lgmath::se3::Transformation, JacobianTreeNode::ConstPtr> evaluateJacobians() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual EvalTreeNode<lgmath::se3::Transformation>* evaluateTree() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the Jacobian tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void appendJacobians(const Eigen::MatrixXd& lhs,
                               EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                               std::vector<Jacobian>* outJacobians) const;

 private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Transformation matrix state variable
  //////////////////////////////////////////////////////////////////////////////////////////////
  se3::TransformStateVar::ConstPtr transform_;

};

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Simple transform evaluator for a fixed transformation
//////////////////////////////////////////////////////////////////////////////////////////////
class FixedTransformEvaluator : public TransformEvaluator
{
 public:

  /// Convenience typedefs
  typedef boost::shared_ptr<FixedTransformEvaluator> Ptr;
  typedef boost::shared_ptr<const FixedTransformEvaluator> ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  FixedTransformEvaluator(const lgmath::se3::Transformation& transform);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Pseudo constructor - return a shared pointer to a new instance
  //////////////////////////////////////////////////////////////////////////////////////////////
  static Ptr MakeShared(const lgmath::se3::Transformation& transform);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Returns whether or not an evaluator contains unlocked state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool isActive() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual lgmath::se3::Transformation evaluate() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix and return empty Jacobian vector
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual lgmath::se3::Transformation evaluate(std::vector<Jacobian>* jacs) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix and return empty Jacobian vector
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual std::pair<lgmath::se3::Transformation, JacobianTreeNode::ConstPtr> evaluateJacobians() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the transformation matrix tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual EvalTreeNode<lgmath::se3::Transformation>* evaluateTree() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the Jacobian tree
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void appendJacobians(const Eigen::MatrixXd& lhs,
                               EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                               std::vector<Jacobian>* outJacobians) const;

 private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Fixed transformation matrix
  //////////////////////////////////////////////////////////////////////////////////////////////
  lgmath::se3::Transformation transform_;

};

} // se3
} // steam

#endif // STEAM_TRANSFORM_EVALUATORS_HPP
