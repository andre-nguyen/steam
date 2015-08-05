//////////////////////////////////////////////////////////////////////////////////////////////
/// \file TransformEvaluators.cpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/evaluator/TransformEvaluators.hpp>

#include <steam/evaluator/jacobian/JacobianTreeLeafNode.hpp>

#include <lgmath.hpp>

namespace steam {
namespace se3 {

// State variable

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
TransformStateEvaluator::TransformStateEvaluator(const se3::TransformStateVar::ConstPtr& transform) : transform_(transform) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Pseudo constructor - return a shared pointer to a new instance
//////////////////////////////////////////////////////////////////////////////////////////////
TransformStateEvaluator::Ptr TransformStateEvaluator::MakeShared(const se3::TransformStateVar::ConstPtr& transform) {
  return TransformStateEvaluator::Ptr(new TransformStateEvaluator(transform));
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Returns whether or not an evaluator contains unlocked state variables
//////////////////////////////////////////////////////////////////////////////////////////////
bool TransformStateEvaluator::isActive() const {
  return !transform_->isLocked();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
lgmath::se3::Transformation TransformStateEvaluator::evaluate() const {
  return transform_->getValue();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix and Jacobian (identity)
//////////////////////////////////////////////////////////////////////////////////////////////
lgmath::se3::Transformation TransformStateEvaluator::evaluate(std::vector<Jacobian>* jacs) const {

  // Check and initialize jacobian array
  if (jacs == NULL) {
    throw std::invalid_argument("Null pointer provided to return-input 'jacs' in evaluate");
  }
  jacs->clear();
  if(!transform_->isLocked()) {
    jacs->resize(1);
    Jacobian& jacref = jacs->back();
    jacref.key = transform_->getKey();
    jacref.jac = Eigen::Matrix<double,6,6>::Identity();
  }
  return transform_->getValue();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix and Jacobian (identity)
//////////////////////////////////////////////////////////////////////////////////////////////
std::pair<lgmath::se3::Transformation, JacobianTreeNode::ConstPtr> TransformStateEvaluator::evaluateJacobians() const {

  // Check if state is locked
  if (transform_->isLocked()) {

    // State is locked, return nullptr in place of leaf node
    return std::make_pair(transform_->getValue(), JacobianTreeLeafNode::Ptr());
  } else {

    // State is unlocked, return a new leaf node
    //return std::make_pair(transform_->getValue(),
    //                      JacobianTreeLeafNode::Ptr(new JacobianTreeLeafNode(transform_)));
    return std::make_pair(transform_->getValue(),
                          boost::make_shared<JacobianTreeLeafNode>(transform_));
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix tree
//////////////////////////////////////////////////////////////////////////////////////////////
EvalTreeNode<lgmath::se3::Transformation>* TransformStateEvaluator::evaluateTree() const {
  return new EvalTreeNode<lgmath::se3::Transformation>(transform_->getValue());
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void TransformStateEvaluator::appendJacobians(const Eigen::MatrixXd& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian>* outJacobians) const {

  // Check if state is locked
  if (!transform_->isLocked()) {

    // Check that dimensions match
    if (lhs.cols() != transform_->getPerturbDim()) {
      throw std::runtime_error("appendJacobians had dimension mismatch.");
    }

    // Add Jacobian
    outJacobians->push_back(Jacobian(transform_->getKey(), lhs));
  }
}

// Fixed value

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
FixedTransformEvaluator::FixedTransformEvaluator(const lgmath::se3::Transformation& transform) : transform_(transform) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Pseudo constructor - return a shared pointer to a new instance
//////////////////////////////////////////////////////////////////////////////////////////////
FixedTransformEvaluator::Ptr FixedTransformEvaluator::MakeShared(const lgmath::se3::Transformation& transform) {
  return FixedTransformEvaluator::Ptr(new FixedTransformEvaluator(transform));
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Returns whether or not an evaluator contains unlocked state variables
//////////////////////////////////////////////////////////////////////////////////////////////
bool FixedTransformEvaluator::isActive() const {
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
lgmath::se3::Transformation FixedTransformEvaluator::evaluate() const {
  return transform_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix and return empty Jacobian vector
//////////////////////////////////////////////////////////////////////////////////////////////
lgmath::se3::Transformation FixedTransformEvaluator::evaluate(std::vector<Jacobian>* jacs) const {
  if (jacs == NULL) {
    throw std::invalid_argument("Null pointer provided to return-input 'jacs' in evaluate");
  }
  jacs->clear(); // no jacobian.. this is a fixed transform
  return transform_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix and return empty Jacobian vector
//////////////////////////////////////////////////////////////////////////////////////////////
std::pair<lgmath::se3::Transformation, JacobianTreeNode::ConstPtr> FixedTransformEvaluator::evaluateJacobians() const {

  // Return nullptr in place of leaf node
  return std::make_pair(transform_, JacobianTreeLeafNode::Ptr());
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix tree
//////////////////////////////////////////////////////////////////////////////////////////////
EvalTreeNode<lgmath::se3::Transformation>* FixedTransformEvaluator::evaluateTree() const {
  return new EvalTreeNode<lgmath::se3::Transformation>(transform_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void FixedTransformEvaluator::appendJacobians(const Eigen::MatrixXd& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian>* outJacobians) const {
  return;
}

} // se3
} // steam
