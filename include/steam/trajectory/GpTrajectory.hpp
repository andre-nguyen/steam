//////////////////////////////////////////////////////////////////////////////////////////////
/// \file GpTrajectory.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_GP_TRAJECTORY_HPP
#define STEAM_GP_TRAJECTORY_HPP

#include <Eigen/Core>

#include <steam/common/Time.hpp>
#include <steam/state/LieGroupStateVar.hpp>
#include <steam/state/VectorSpaceStateVar.hpp>
#include <steam/evaluator/blockauto/transform/TransformEvaluator.hpp>
#include <steam/problem/CostTerm.hpp>
#include <steam/problem/CostTermCollection.hpp>

namespace steam {
namespace se3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The trajectory class wraps a set of state variables to provide an interface
///        that allows for continuous-time pose interpolation.
//////////////////////////////////////////////////////////////////////////////////////////////
class GpTrajectory
{
 public:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Structure to hold data associated with each knot
  //////////////////////////////////////////////////////////////////////////////////////////////
  struct Knot {

    /// Convenience typedefs
    typedef boost::shared_ptr<Knot> Ptr;
    typedef boost::shared_ptr<const Knot> ConstPtr;

    // Pose
    se3::TransformEvaluator::Ptr T_k_root;

    // Velocity
    VectorSpaceStateVar::Ptr varpi;

    // Time
    steam::Time time;
  };

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  ///        Note, without providing Qc, the trajectory can be used safely for interpolation,
  ///        but should not be used for estimation.
  //////////////////////////////////////////////////////////////////////////////////////////////
  GpTrajectory(bool allowExtrapolation = false);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  GpTrajectory(const Eigen::Matrix<double,6,6>& Qc_inv, bool allowExtrapolation = false);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Add a new knot
  //////////////////////////////////////////////////////////////////////////////////////////////
  void add(const steam::Time& time, const se3::TransformEvaluator::Ptr& T_k0, const VectorSpaceStateVar::Ptr& varpi);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get evaluator
  //////////////////////////////////////////////////////////////////////////////////////////////
  TransformEvaluator::ConstPtr getEvaluator(const steam::Time& time) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get a unary velocity prior
  //////////////////////////////////////////////////////////////////////////////////////////////
  //void getVelocityPriorFactor(const steam::Time& time, CostTermCollection<6,6>::Ptr unary) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get binary cost terms associated with the prior for active parts of the trajectory
  //////////////////////////////////////////////////////////////////////////////////////////////
  void getBinaryPriorFactors(const CostTermCollectionX::Ptr& binary) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get active state variables in the trajectory
  //////////////////////////////////////////////////////////////////////////////////////////////
  void getActiveStateVariables(
      std::map<unsigned int, steam::StateVariableBase::Ptr>* outStates) const;

 private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Ordered map of knots
  //////////////////////////////////////////////////////////////////////////////////////////////
  Eigen::Matrix<double,6,6> Qc_inv_;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Allow for extrapolation
  //////////////////////////////////////////////////////////////////////////////////////////////
  bool allowExtrapolation_;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Ordered map of knots
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::map<boost::int64_t, Knot::Ptr> knotMap_;

};

} // se3
} // steam

#endif // STEAM_GP_TRAJECTORY_HPP
