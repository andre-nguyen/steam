//////////////////////////////////////////////////////////////////////////////////////////////
/// \file StateVariable.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_STATE_VARIABLE_HPP
#define STEAM_STATE_VARIABLE_HPP

#include <steam/state/StateVariableBase.hpp>

namespace steam {

/////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Basic state variable interface.
///        Introduces a (templated) value, with a 'get' and setFromCopy function.
/////////////////////////////////////////////////////////////////////////////////////////////
template<typename TYPE>
class StateVariable : public StateVariableBase
{
 public:

  /// Convenience typedefs
  typedef std::shared_ptr<StateVariable<TYPE> > Ptr;
  typedef std::shared_ptr<const StateVariable<TYPE> > ConstPtr;

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor based on perturbation dimension
  /////////////////////////////////////////////////////////////////////////////////////////////
  StateVariable(unsigned int perturbDim);

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Construct from TYPE instance and perturbation dimension
  /////////////////////////////////////////////////////////////////////////////////////////////
  StateVariable(TYPE t, unsigned int perturbDim);

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Destructor
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~StateVariable() {}

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get current value
  /////////////////////////////////////////////////////////////////////////////////////////////
  const TYPE& getValue() const;

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Set value from another instance of the state variable
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual void setFromCopy(const StateVariableBase::ConstPtr& other);

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface to update a state from a perturbation
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool update(const Eigen::VectorXd& perturbation) = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Interface for clone method
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual StateVariableBase::Ptr clone() const = 0;

 protected:

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Instance of TYPE
  /////////////////////////////////////////////////////////////////////////////////////////////
  TYPE value_;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // steam

#include <steam/state/StateVariable-inl.hpp>

#endif // STEAM_STATE_VARIABLE_HPP
