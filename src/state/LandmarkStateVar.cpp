//////////////////////////////////////////////////////////////////////////////////////////////
/// \file LandmarkStateVar.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/state/LandmarkStateVar.hpp>

namespace steam {
namespace se3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor from a global 3D point
//////////////////////////////////////////////////////////////////////////////////////////////
LandmarkStateVar::LandmarkStateVar(const Eigen::Vector3d& v_0) : StateVariable(3) {
  this->value_.head<3>() = v_0;
  this->value_[3] = 1.0; // fourth element
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor from a 3D point with a reference frame
//////////////////////////////////////////////////////////////////////////////////////////////
LandmarkStateVar::LandmarkStateVar(const Eigen::Vector3d& v_ref,
                                   const TransformEvaluator::ConstPtr& refFrame)
  : StateVariable(3), refFrame_(refFrame) {
  this->value_.head<3>() = v_ref;
  this->value_[3] = 1.0; // fourth element
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Update the landmark state from the 3-dimensional perturbation
//////////////////////////////////////////////////////////////////////////////////////////////
bool LandmarkStateVar::update(const Eigen::VectorXd& perturbation) {

  if (perturbation.size() != this->getPerturbDim()) {
    throw std::runtime_error("During attempt to update a state variable, the provided "
                             "perturbation (VectorXd) was not the correct size.");
  }

  this->value_.head<3>() = this->value_.head<3>() + perturbation; // todo: speed this up ? http://eigen.tuxfamily.org/dox/TopicWritingEfficientProductExpression.html
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Clone method
//////////////////////////////////////////////////////////////////////////////////////////////
StateVariableBase::Ptr LandmarkStateVar::clone() const {
  return LandmarkStateVar::Ptr(new LandmarkStateVar(*this));
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Check if a reference frame was set for the landmark.
//////////////////////////////////////////////////////////////////////////////////////////////
bool LandmarkStateVar::hasReferenceFrame() const {
  return bool(refFrame_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Set value -- mostly for landmark initialization
//////////////////////////////////////////////////////////////////////////////////////////////
void LandmarkStateVar::set(const Eigen::Vector3d& v) {
  this->value_.head<3>() = v;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the reference frame transform evaluator
//////////////////////////////////////////////////////////////////////////////////////////////
const TransformEvaluator::ConstPtr& LandmarkStateVar::getReferenceFrame() const {
  return refFrame_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get point transformed into the global (or base) frame.
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector4d LandmarkStateVar::getGlobalValue() const {
  if (this->hasReferenceFrame()) {
    return refFrame_->evaluate().inverse() * this->value_;
  } else {
    return this->value_;
  }
}

} // se3
} // steam
