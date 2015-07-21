//////////////////////////////////////////////////////////////////////////////////////////////
/// \file VectorSpaceStateVar.cpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/state/VectorSpaceStateVar.hpp>
#include <glog/logging.h>

namespace steam {

/////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Construct from Eigen vector (perturbation dimension assumed to match vector)
/////////////////////////////////////////////////////////////////////////////////////////////
VectorSpaceStateVar::VectorSpaceStateVar(Eigen::VectorXd v) : StateVariable(v, v.size()) {}

/////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Update the vector space state variable via an additive perturbation:
///          this += perturbation
/////////////////////////////////////////////////////////////////////////////////////////////
bool VectorSpaceStateVar::update(const Eigen::VectorXd& perturbation) {
  CHECK(perturbation.size() == this->getPerturbDim());
  this->value_ = this->value_ + perturbation;
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Clone method
/////////////////////////////////////////////////////////////////////////////////////////////
StateVariableBase::Ptr VectorSpaceStateVar::clone() const {
  return VectorSpaceStateVar::Ptr(new VectorSpaceStateVar(*this));
}


} // steam
