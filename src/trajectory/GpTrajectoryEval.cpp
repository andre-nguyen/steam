//////////////////////////////////////////////////////////////////////////////////////////////
/// \file TransformEvaluators.cpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/trajectory/GpTrajectoryEval.hpp>

#include <lgmath.hpp>

namespace steam {
namespace se3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
GpTrajectoryEval::GpTrajectoryEval(const Time& time, const GpTrajectory::Knot::ConstPtr& knot1,
                                   const GpTrajectory::Knot::ConstPtr& knot2) :
  knot1_(knot1), knot2_(knot2) {

  // Calculate time constants
  double tau = (time - knot1->time).seconds();
  double T = (knot2->time - knot1->time).seconds();
  double ratio = tau/T;
  double ratio2 = ratio*ratio;
  double ratio3 = ratio2*ratio;

  // Calculate 'psi' interpolation values
  psi11_ = 3.0*ratio2 - 2.0*ratio3;
  psi12_ = tau*(ratio2 - ratio);
  psi21_ = 6.0*(ratio - ratio2)/T;
  psi22_ = 3.0*ratio2 - 2.0*ratio;

  // Calculate 'lambda' interpolation values
  lambda11_ = 1.0 - psi11_;
  lambda12_ = tau - T*psi11_ - psi12_;
  lambda21_ = -psi21_;
  lambda22_ = 1.0 - psi21_ - psi22_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Pseudo constructor - return a shared pointer to a new instance
//////////////////////////////////////////////////////////////////////////////////////////////
GpTrajectoryEval::Ptr GpTrajectoryEval::MakeShared(const Time& time, const GpTrajectory::Knot::ConstPtr& knot1,
                                                   const GpTrajectory::Knot::ConstPtr& knot2) {
  return GpTrajectoryEval::Ptr(new GpTrajectoryEval(time, knot1, knot2));
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Returns whether or not an evaluator contains unlocked state variables
//////////////////////////////////////////////////////////////////////////////////////////////
bool GpTrajectoryEval::isActive() const {
  return knot1_->T_k_root->isActive()  ||
         !knot1_->varpi->isLocked() ||
         knot2_->T_k_root->isActive()  ||
         !knot2_->varpi->isLocked();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
lgmath::se3::Transformation GpTrajectoryEval::evaluate() const {

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = knot2_->T_k_root->evaluate()/knot1_->T_k_root->evaluate();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Return `global' interpolated transform
  return T_i1*knot1_->T_k_root->evaluate();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the transformation matrix tree
//////////////////////////////////////////////////////////////////////////////////////////////
EvalTreeNode<lgmath::se3::Transformation>* GpTrajectoryEval::evaluateTree() const {

  // Evaluate sub-trees
  EvalTreeNode<lgmath::se3::Transformation>* transform1 = knot1_->T_k_root->evaluateTree();
  EvalTreeNode<lgmath::se3::Transformation>* transform2 = knot2_->T_k_root->evaluateTree();

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Interpolated relative transform - new root node (using pool memory)
  EvalTreeNode<lgmath::se3::Transformation>* root = EvalTreeNode<lgmath::se3::Transformation>::pool.getObj();
  root->setValue(T_i1*transform1->getValue());

  // Add children
  root->addChild(transform1);
  root->addChild(transform2);

  // Return new root node
  return root;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Evaluate the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void GpTrajectoryEval::appendJacobians(const Eigen::MatrixXd& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian<> >* outJacobians) const {

  // Cast back to transformations
  EvalTreeNode<lgmath::se3::Transformation>* transform1 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(0));
  EvalTreeNode<lgmath::se3::Transformation>* transform2 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(1));

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Calculate the 6x6 Jacobian associated with the interpolated relative transformation matrix
  Eigen::Matrix<double,6,6> J_i1 = lgmath::se3::vec2jac(xi_i1);

  // Check if evaluator is active
  if (this->isActive()) {

    // Pose Jacobians
    if (knot1_->T_k_root->isActive() || knot2_->T_k_root->isActive()) {

      // Precompute matrix
      Eigen::Matrix<double,6,6> w = psi11_*J_i1*J_21_inv +
        0.5*psi12_*J_i1*lgmath::se3::curlyhat(knot2_->varpi->getValue())*J_21_inv;

      // Check if transform1 is active
      if (knot1_->T_k_root->isActive()) {
        Eigen::Matrix<double,6,6> jacobian = (-1) * w * T_21.adjoint() + T_i1.adjoint();
        knot1_->T_k_root->appendJacobians(lhs*jacobian, transform1, outJacobians);
      }

      // Get index of split between left and right-hand-side of Jacobians
      unsigned int hintIndex = outJacobians->size();

      // Check if transform2 is active
      if (knot2_->T_k_root->isActive()) {
        knot2_->T_k_root->appendJacobians(lhs*w, transform2, outJacobians);
      }

      // Merge jacobians
      Jacobian<>::merge(outJacobians, hintIndex);
    }

    // 6 x 6 Velocity Jacobian 1
    if(!knot1_->varpi->isLocked()) {

      // Add Jacobian
      outJacobians->push_back(Jacobian<>(knot1_->varpi->getKey(), lhs*lambda12_*J_i1));
    }

    // 6 x 6 Velocity Jacobian 2
    if(!knot2_->varpi->isLocked()) {

      // Add Jacobian
      Eigen::Matrix<double,6,6> jacobian = psi12_*J_i1*J_21_inv;
      outJacobians->push_back(Jacobian<>(knot2_->varpi->getKey(), lhs*jacobian));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fixed-size evaluations of the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void GpTrajectoryEval::appendJacobians1(const Eigen::Matrix<double,1,6>& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian<1,6> >* outJacobians) const {

  // Cast back to transformations
  EvalTreeNode<lgmath::se3::Transformation>* transform1 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(0));
  EvalTreeNode<lgmath::se3::Transformation>* transform2 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(1));

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Calculate the 6x6 Jacobian associated with the interpolated relative transformation matrix
  Eigen::Matrix<double,6,6> J_i1 = lgmath::se3::vec2jac(xi_i1);

  // Check if evaluator is active
  if (this->isActive()) {

    // Pose Jacobians
    if (knot1_->T_k_root->isActive() || knot2_->T_k_root->isActive()) {

      // Precompute matrix
      Eigen::Matrix<double,6,6> w = psi11_*J_i1*J_21_inv +
        0.5*psi12_*J_i1*lgmath::se3::curlyhat(knot2_->varpi->getValue())*J_21_inv;

      // Check if transform1 is active
      if (knot1_->T_k_root->isActive()) {
        Eigen::Matrix<double,6,6> jacobian = (-1) * w * T_21.adjoint() + T_i1.adjoint();
        knot1_->T_k_root->appendJacobians1(lhs*jacobian, transform1, outJacobians);
      }

      // Get index of split between left and right-hand-side of Jacobians
      unsigned int hintIndex = outJacobians->size();

      // Check if transform2 is active
      if (knot2_->T_k_root->isActive()) {
        knot2_->T_k_root->appendJacobians1(lhs*w, transform2, outJacobians);
      }

      // Merge jacobians
      Jacobian<1,6>::merge(outJacobians, hintIndex);
    }

    // 6 x 6 Velocity Jacobian 1
    if(!knot1_->varpi->isLocked()) {

      // Add Jacobian
      outJacobians->push_back(Jacobian<1,6>(knot1_->varpi->getKey(), lhs*lambda12_*J_i1));
    }

    // 6 x 6 Velocity Jacobian 2
    if(!knot2_->varpi->isLocked()) {

      // Add Jacobian
      Eigen::Matrix<double,6,6> jacobian = psi12_*J_i1*J_21_inv;
      outJacobians->push_back(Jacobian<1,6>(knot2_->varpi->getKey(), lhs*jacobian));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fixed-size evaluations of the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void GpTrajectoryEval::appendJacobians2(const Eigen::Matrix<double,2,6>& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian<2,6> >* outJacobians) const {

  // Cast back to transformations
  EvalTreeNode<lgmath::se3::Transformation>* transform1 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(0));
  EvalTreeNode<lgmath::se3::Transformation>* transform2 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(1));

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Calculate the 6x6 Jacobian associated with the interpolated relative transformation matrix
  Eigen::Matrix<double,6,6> J_i1 = lgmath::se3::vec2jac(xi_i1);

  // Check if evaluator is active
  if (this->isActive()) {

    // Pose Jacobians
    if (knot1_->T_k_root->isActive() || knot2_->T_k_root->isActive()) {

      // Precompute matrix
      Eigen::Matrix<double,6,6> w = psi11_*J_i1*J_21_inv +
        0.5*psi12_*J_i1*lgmath::se3::curlyhat(knot2_->varpi->getValue())*J_21_inv;

      // Check if transform1 is active
      if (knot1_->T_k_root->isActive()) {
        Eigen::Matrix<double,6,6> jacobian = (-1) * w * T_21.adjoint() + T_i1.adjoint();
        knot1_->T_k_root->appendJacobians2(lhs*jacobian, transform1, outJacobians);
      }

      // Get index of split between left and right-hand-side of Jacobians
      unsigned int hintIndex = outJacobians->size();

      // Check if transform2 is active
      if (knot2_->T_k_root->isActive()) {
        knot2_->T_k_root->appendJacobians2(lhs*w, transform2, outJacobians);
      }

      // Merge jacobians
      Jacobian<2,6>::merge(outJacobians, hintIndex);
    }

    // 6 x 6 Velocity Jacobian 1
    if(!knot1_->varpi->isLocked()) {

      // Add Jacobian
      outJacobians->push_back(Jacobian<2,6>(knot1_->varpi->getKey(), lhs*lambda12_*J_i1));
    }

    // 6 x 6 Velocity Jacobian 2
    if(!knot2_->varpi->isLocked()) {

      // Add Jacobian
      Eigen::Matrix<double,6,6> jacobian = psi12_*J_i1*J_21_inv;
      outJacobians->push_back(Jacobian<2,6>(knot2_->varpi->getKey(), lhs*jacobian));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fixed-size evaluations of the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void GpTrajectoryEval::appendJacobians3(const Eigen::Matrix<double,3,6>& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian<3,6> >* outJacobians) const {

  // Cast back to transformations
  EvalTreeNode<lgmath::se3::Transformation>* transform1 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(0));
  EvalTreeNode<lgmath::se3::Transformation>* transform2 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(1));

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Calculate the 6x6 Jacobian associated with the interpolated relative transformation matrix
  Eigen::Matrix<double,6,6> J_i1 = lgmath::se3::vec2jac(xi_i1);

  // Check if evaluator is active
  if (this->isActive()) {

    // Pose Jacobians
    if (knot1_->T_k_root->isActive() || knot2_->T_k_root->isActive()) {

      // Precompute matrix
      Eigen::Matrix<double,6,6> w = psi11_*J_i1*J_21_inv +
        0.5*psi12_*J_i1*lgmath::se3::curlyhat(knot2_->varpi->getValue())*J_21_inv;

      // Check if transform1 is active
      if (knot1_->T_k_root->isActive()) {
        Eigen::Matrix<double,6,6> jacobian = (-1) * w * T_21.adjoint() + T_i1.adjoint();
        knot1_->T_k_root->appendJacobians3(lhs*jacobian, transform1, outJacobians);
      }

      // Get index of split between left and right-hand-side of Jacobians
      unsigned int hintIndex = outJacobians->size();

      // Check if transform2 is active
      if (knot2_->T_k_root->isActive()) {
        knot2_->T_k_root->appendJacobians3(lhs*w, transform2, outJacobians);
      }

      // Merge jacobians
      Jacobian<3,6>::merge(outJacobians, hintIndex);
    }

    // 6 x 6 Velocity Jacobian 1
    if(!knot1_->varpi->isLocked()) {

      // Add Jacobian
      outJacobians->push_back(Jacobian<3,6>(knot1_->varpi->getKey(), lhs*lambda12_*J_i1));
    }

    // 6 x 6 Velocity Jacobian 2
    if(!knot2_->varpi->isLocked()) {

      // Add Jacobian
      Eigen::Matrix<double,6,6> jacobian = psi12_*J_i1*J_21_inv;
      outJacobians->push_back(Jacobian<3,6>(knot2_->varpi->getKey(), lhs*jacobian));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fixed-size evaluations of the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void GpTrajectoryEval::appendJacobians4(const Eigen::Matrix<double,4,6>& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian<4,6> >* outJacobians) const {

  // Cast back to transformations
  EvalTreeNode<lgmath::se3::Transformation>* transform1 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(0));
  EvalTreeNode<lgmath::se3::Transformation>* transform2 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(1));

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Calculate the 6x6 Jacobian associated with the interpolated relative transformation matrix
  Eigen::Matrix<double,6,6> J_i1 = lgmath::se3::vec2jac(xi_i1);

  // Check if evaluator is active
  if (this->isActive()) {

    // Pose Jacobians
    if (knot1_->T_k_root->isActive() || knot2_->T_k_root->isActive()) {

      // Precompute matrix
      Eigen::Matrix<double,6,6> w = psi11_*J_i1*J_21_inv +
        0.5*psi12_*J_i1*lgmath::se3::curlyhat(knot2_->varpi->getValue())*J_21_inv;

      // Check if transform1 is active
      if (knot1_->T_k_root->isActive()) {
        Eigen::Matrix<double,6,6> jacobian = (-1) * w * T_21.adjoint() + T_i1.adjoint();
        knot1_->T_k_root->appendJacobians4(lhs*jacobian, transform1, outJacobians);
      }

      // Get index of split between left and right-hand-side of Jacobians
      unsigned int hintIndex = outJacobians->size();

      // Check if transform2 is active
      if (knot2_->T_k_root->isActive()) {
        knot2_->T_k_root->appendJacobians4(lhs*w, transform2, outJacobians);
      }

      // Merge jacobians
      Jacobian<4,6>::merge(outJacobians, hintIndex);
    }

    // 6 x 6 Velocity Jacobian 1
    if(!knot1_->varpi->isLocked()) {

      // Add Jacobian
      outJacobians->push_back(Jacobian<4,6>(knot1_->varpi->getKey(), lhs*lambda12_*J_i1));
    }

    // 6 x 6 Velocity Jacobian 2
    if(!knot2_->varpi->isLocked()) {

      // Add Jacobian
      Eigen::Matrix<double,6,6> jacobian = psi12_*J_i1*J_21_inv;
      outJacobians->push_back(Jacobian<4,6>(knot2_->varpi->getKey(), lhs*jacobian));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fixed-size evaluations of the Jacobian tree
//////////////////////////////////////////////////////////////////////////////////////////////
void GpTrajectoryEval::appendJacobians6(const Eigen::Matrix<double,6,6>& lhs,
                             EvalTreeNode<lgmath::se3::Transformation>* evaluationTree,
                             std::vector<Jacobian<6,6> >* outJacobians) const {

  // Cast back to transformations
  EvalTreeNode<lgmath::se3::Transformation>* transform1 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(0));
  EvalTreeNode<lgmath::se3::Transformation>* transform2 =
      static_cast<EvalTreeNode<lgmath::se3::Transformation>*>(evaluationTree->childAt(1));

  // Get relative matrix info
  lgmath::se3::Transformation T_21 = transform2->getValue()/transform1->getValue();

  // Get se3 algebra of relative matrix
  Eigen::Matrix<double,6,1> xi_21 = T_21.vec();

  // Calculate the 6x6 associated Jacobian
  Eigen::Matrix<double,6,6> J_21_inv = lgmath::se3::vec2jacinv(xi_21);

  // Calculate interpolated relative se3 algebra
  Eigen::Matrix<double,6,1> xi_i1 = lambda12_*knot1_->varpi->getValue() +
                                    psi11_*xi_21 +
                                    psi12_*J_21_inv*knot2_->varpi->getValue();

  // Calculate interpolated relative transformation matrix
  lgmath::se3::Transformation T_i1(xi_i1);

  // Calculate the 6x6 Jacobian associated with the interpolated relative transformation matrix
  Eigen::Matrix<double,6,6> J_i1 = lgmath::se3::vec2jac(xi_i1);

  // Check if evaluator is active
  if (this->isActive()) {

    // Pose Jacobians
    if (knot1_->T_k_root->isActive() || knot2_->T_k_root->isActive()) {

      // Precompute matrix
      Eigen::Matrix<double,6,6> w = psi11_*J_i1*J_21_inv +
        0.5*psi12_*J_i1*lgmath::se3::curlyhat(knot2_->varpi->getValue())*J_21_inv;

      // Check if transform1 is active
      if (knot1_->T_k_root->isActive()) {
        Eigen::Matrix<double,6,6> jacobian = (-1) * w * T_21.adjoint() + T_i1.adjoint();
        knot1_->T_k_root->appendJacobians6(lhs*jacobian, transform1, outJacobians);
      }

      // Get index of split between left and right-hand-side of Jacobians
      unsigned int hintIndex = outJacobians->size();

      // Check if transform2 is active
      if (knot2_->T_k_root->isActive()) {
        knot2_->T_k_root->appendJacobians6(lhs*w, transform2, outJacobians);
      }

      // Merge jacobians
      Jacobian<6,6>::merge(outJacobians, hintIndex);
    }

    // 6 x 6 Velocity Jacobian 1
    if(!knot1_->varpi->isLocked()) {

      // Add Jacobian
      outJacobians->push_back(Jacobian<6,6>(knot1_->varpi->getKey(), lhs*lambda12_*J_i1));
    }

    // 6 x 6 Velocity Jacobian 2
    if(!knot2_->varpi->isLocked()) {

      // Add Jacobian
      Eigen::Matrix<double,6,6> jacobian = psi12_*J_i1*J_21_inv;
      outJacobians->push_back(Jacobian<6,6>(knot2_->varpi->getKey(), lhs*jacobian));
    }
  }
}

} // se3
} // steam
