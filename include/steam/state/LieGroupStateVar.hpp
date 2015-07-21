//////////////////////////////////////////////////////////////////////////////////////////////
/// \file LieGroupStateVar.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_LIE_GROUP_STATE_VARIABLE_HPP
#define STEAM_LIE_GROUP_STATE_VARIABLE_HPP

#include <steam/state/StateVariable.hpp>
#include <lgmath/Transformation.hpp>

namespace steam {

/////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Lie group state variable. Note that TYPE must implement a constructor from a
///        vector of the perturbation dimension (DIM); usually this is the exponential map.
/////////////////////////////////////////////////////////////////////////////////////////////
template<typename TYPE, int DIM>
class LieGroupStateVar : public StateVariable<TYPE>
{
 public:

  /// Convenience typedefs
  typedef boost::shared_ptr<LieGroupStateVar<TYPE,DIM> > Ptr;
  typedef boost::shared_ptr<const LieGroupStateVar<TYPE,DIM> > ConstPtr;

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Default constructor
  /////////////////////////////////////////////////////////////////////////////////////////////
  LieGroupStateVar();

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Construct from an instance of the value type
  /////////////////////////////////////////////////////////////////////////////////////////////
  LieGroupStateVar(const TYPE& t); // construct directly from matrix type

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Construct from a vector (through some exponential map)
  /////////////////////////////////////////////////////////////////////////////////////////////
  LieGroupStateVar(const Eigen::Matrix<double,DIM,1>& v);

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Destructor
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~LieGroupStateVar() {}

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Update the lie group state variable via a left-hand multiplicative perturbation:
  ///          this = TYPE(perturbation) * this
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool update(const Eigen::VectorXd& perturbation);

  /////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Clone method
  /////////////////////////////////////////////////////////////////////////////////////////////
  virtual StateVariableBase::Ptr clone() const;

};

/////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Define a transformation matrix state based on the lgmath library
/////////////////////////////////////////////////////////////////////////////////////////////
namespace se3 {
  typedef LieGroupStateVar<lgmath::se3::Transformation,6> TransformStateVar;
}

//namespace so3 {
//  typedef LieGroupStateVar<lgmath::so3::Rotation,3> RotationStateVar;
//}

//namespace se2 {
//  typedef LieGroupStateVar<lgmath::se2::Transformation,3> TransformStateVar;
//}

//namespace so2 {
//  typedef LieGroupStateVar<lgmath::so2::Rotation,1> RotationStateVar;
//}

} // steam

#include <steam/state/LieGroupStateVar-inl.hpp>

#endif // STEAM_LIE_GROUP_STATE_VARIABLE_HPP
