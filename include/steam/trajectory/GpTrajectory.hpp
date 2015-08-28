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
#include <steam/evaluator/TransformEvaluators.hpp>
#include <steam/problem/CostTerm.hpp>
#include <steam/problem/CostTermCollection.hpp>

namespace steam {
namespace se3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Simple transform evaluator for a transformation state variable
//////////////////////////////////////////////////////////////////////////////////////////////
class GpTrajectory
{
 public:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Structure to hold data associated with each knot
  //////////////////////////////////////////////////////////////////////////////////////////////
//  struct Knot {

//    /// Convenience typedefs
//    typedef boost::shared_ptr<Knot> Ptr;
//    typedef boost::shared_ptr<const Knot> ConstPtr;

//    // Pose
//    se3::TransformStateVar::Ptr T_k0;

//    // Velocity
//    VectorSpaceStateVar::Ptr varpi;

//    // Time
//    steam::Time time;
//  };

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

  // Notes
  // - For now we only allow adding.. this way pose evaluators stay valid. In the future, the
  //   pose evaluator could simply store a reference back to the GpTrajectory, and the
  //   evaluation methods could be called from the trajectory .. the overhead
  //   being the std::map search time.

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  GpTrajectory(const Eigen::Matrix<double,6,6>& Qc_inv);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Add a new knot
  //////////////////////////////////////////////////////////////////////////////////////////////
  void add(const steam::Time& time, const se3::TransformEvaluator::Ptr& T_k0, const VectorSpaceStateVar::Ptr& varpi);

//  //////////////////////////////////////////////////////////////////////////////////////////////
//  /// \brief Add a new knot
//  //////////////////////////////////////////////////////////////////////////////////////////////
//  void add(const steam::Time& time, const lgmath::se3::Transformation& T_k0, const Eigen::Matrix<double,6,1>& varpi);

//  //////////////////////////////////////////////////////////////////////////////////////////////
//  /// \brief Add a new knot. Initialize varpi using constant velocity between this and last pose
//  //////////////////////////////////////////////////////////////////////////////////////////////
//  void add(const steam::Time& time, const lgmath::se3::Transformation& T_k0);

//  //////////////////////////////////////////////////////////////////////////////////////////////
//  /// \brief Add a new knot. Initialize by extrapolating velocity estimate.
//  //////////////////////////////////////////////////////////////////////////////////////////////
//  void add(const steam::Time& time);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get evaluator
  //////////////////////////////////////////////////////////////////////////////////////////////
  TransformEvaluator::ConstPtr getEvaluator(const steam::Time& time) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Locks state variables before the provided time. Useful in sliding window filters.
  //////////////////////////////////////////////////////////////////////////////////////////////
  //void lockBefore(const steam::Time& time);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get unlocked state variables in the trajectory
  //////////////////////////////////////////////////////////////////////////////////////////////
  //std::vector<steam::StateVariableBase::Ptr> getActiveStateVariables() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get cost terms associated with the prior for unlocked parts of the trajectory
  //////////////////////////////////////////////////////////////////////////////////////////////
  void getPriorCostTerms(CostTermCollection<6,6>::Ptr unary,
                         CostTermCollectionX::Ptr binary) const;


  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get all of the transformation state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  //std::vector<se3::TransformStateVar::Ptr> getTransformStateVariables() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get all of the velocity state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  //std::vector<VectorSpaceStateVar::Ptr> getVelocityStateVariables() const;

 private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Ordered map of knots
  //////////////////////////////////////////////////////////////////////////////////////////////
  Eigen::Matrix<double,6,6> Qc_inv_;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Ordered map of knots
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::map<boost::int64_t, Knot::Ptr> knotMap_;

};

} // se3
} // steam

#endif // STEAM_GP_TRAJECTORY_HPP
