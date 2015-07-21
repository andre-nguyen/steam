//////////////////////////////////////////////////////////////////////////////////////////////
/// \file VectorSpaceErrorEval.cpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/evaluator/VectorSpaceErrorEval.hpp>

#include <glog/logging.h>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
VectorSpaceErrorEval::VectorSpaceErrorEval(const Eigen::VectorXd& measurement, const VectorSpaceStateVar::ConstPtr& stateVec)
  : measurement_(measurement), stateVec_(stateVec) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Returns whether or not an evaluator contains unlocked state variables
//////////////////////////////////////////////////////////////////////////////////////////////
bool VectorSpaceErrorEval::isActive() const {
  return !stateVec_->isLocked();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the measurement error
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXd VectorSpaceErrorEval::evaluate() const {
  return measurement_ - stateVec_->getValue();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the measurement error and relevant Jacobians
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXd VectorSpaceErrorEval::evaluate(std::vector<Jacobian>* jacs) const {

  // Check and initialize jacobian array
  CHECK_NOTNULL(jacs);
  jacs->clear();

  // Construct Jacobian
  if(!stateVec_->isLocked()) {
    jacs->resize(1);
    (*jacs)[0].key = stateVec_->getKey();
    const unsigned int dim = stateVec_->getPerturbDim();
    (*jacs)[0].jac = -Eigen::MatrixXd::Identity(dim,dim);
  }

  // Return error
  return measurement_ - stateVec_->getValue();
}

} // steam
