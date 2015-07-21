//////////////////////////////////////////////////////////////////////////////////////////////
/// \file OptimizationProblem.cpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/OptimizationProblem.hpp>

#include <iomanip>
#include <glog/logging.h>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Default Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
OptimizationProblem::OptimizationProblem() : pendingProposedState_(false) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Add an 'active' state variable
//////////////////////////////////////////////////////////////////////////////////////////////
void OptimizationProblem::addStateVariable(const StateVariableBase::Ptr& state)
{
  stateVec_.addStateVariable(state);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Add a cost term (should depend on active states that were added to the problem)
//////////////////////////////////////////////////////////////////////////////////////////////
void OptimizationProblem::addCostTerm(const CostTerm::ConstPtr& costTerm) {
  costTerms_.push_back(costTerm);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Compute the cost from the collection of cost terms
//////////////////////////////////////////////////////////////////////////////////////////////
double OptimizationProblem::cost() const {
  double cost = 0.0;
  for(std::vector<CostTerm::ConstPtr>::const_iterator it = costTerms_.begin();
      it != costTerms_.end(); ++it) {
    cost += (*it)->evaluate();
  }
  return cost;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get a reference to the state vector
//////////////////////////////////////////////////////////////////////////////////////////////
const StateVector& OptimizationProblem::getStateVector() const {
  return stateVec_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get a reference to the cost terms
//////////////////////////////////////////////////////////////////////////////////////////////
const std::vector<CostTerm::ConstPtr>& OptimizationProblem::getCostTerms() const {
  return costTerms_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Propose an update to the state vector.
//////////////////////////////////////////////////////////////////////////////////////////////
double OptimizationProblem::proposeUpdate(const Eigen::VectorXd& stateStep) {

  // Check that an update is not already pending
  CHECK(pendingProposedState_ != true) <<
    "There is already a pending update, accept or reject before proposing a new one.";

  // Make copy of state vector
  stateVectorBackup_ = stateVec_;

  // Update copy with perturbation
  stateVec_.update(stateStep);
  pendingProposedState_ = true;

  // Test new cost
  return this->cost();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Confirm the proposed state update
//////////////////////////////////////////////////////////////////////////////////////////////
void OptimizationProblem::acceptProposedState() {

  // Check that an update has been proposed
  CHECK(pendingProposedState_ == true) << "You must call proposeUpdate before accept.";

  // Switch flag, accepting the update
  pendingProposedState_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Reject the proposed state update and revert to the previous values
//////////////////////////////////////////////////////////////////////////////////////////////
void OptimizationProblem::rejectProposedState() {

  // Check that an update has been proposed
  CHECK(pendingProposedState_ == true) << "You must call proposeUpdate before rejecting.";

  // Revert to previous state
  stateVec_.copyValues(stateVectorBackup_);

  // Switch flag, ready for new proposal
  pendingProposedState_ = false;
}



} // steam
