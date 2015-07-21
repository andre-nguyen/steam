//////////////////////////////////////////////////////////////////////////////////////////////
/// \file SpherePoseGraphRelax.cpp
/// \brief A sample usage of the STEAM Engine library for solving the iSAM1 spherical pose graph
///        relaxation problem.
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <glog/logging.h>

#include <lgmath/Transformation.hpp>

#include <steam/data/ParseSphere.hpp>
#include <steam/OptimizationProblem.hpp>
#include <steam/solver/VanillaGaussNewtonSolver.hpp>
#include <steam/evaluator/TransformErrorEval.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Example that loads and solves an iSAM1 spherical pose graph problem
//////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

  // Init glog
  google::InitGoogleLogging(argv[0]);

  ///
  /// Parse Dataset - sphere of relative pose measurements (fairly dense loop closures)
  ///

  // Get filename
  std::string filename;
  if (argc < 2) {
    filename = "../../include/steam/data/sphere2500.txt";
    std::cout << "Parsing default file: " << filename << std::endl << std::endl;
  } else {
    filename = argv[1];
    std::cout << "Parsing file: " << filename << std::endl << std::endl;
  }

  std::vector<steam::data::SphereEdge> measCollection = steam::data::parseSphereDataset(filename);

  ///
  /// Setup and Initialize States
  ///

  // steam state variables
  std::vector<steam::se3::TransformStateVar::Ptr> poses_k_0;

  // Edges (for graphics)
  std::vector<std::pair<int,int> > edges;

  // Add initial state
  {
    steam::se3::TransformStateVar::Ptr pose_0_0(new steam::se3::TransformStateVar());

    // Lock first pose (otherwise entire solution is 'floating')
    //  **Note: alternatively we could add a prior (UnaryTransformError) to the first pose.
    pose_0_0->setLock(true);

    poses_k_0.push_back(pose_0_0);
  }

  // Add states from odometry
  for (unsigned int i = 0; i < measCollection.size(); i++) {

    // Looping through all measurements (including loop closures), check if measurement provides odometry
    if (measCollection[i].idA == poses_k_0.size()-1 &&
        measCollection[i].idB == poses_k_0.size()) {

      lgmath::se3::Transformation T_k_0 = measCollection[i].T_BA * poses_k_0[poses_k_0.size()-1]->getValue();
      steam::se3::TransformStateVar::Ptr temp(new steam::se3::TransformStateVar(T_k_0));
      poses_k_0.push_back(temp);
    }

    // Add edge graphic
    edges.push_back(std::make_pair(measCollection[i].idA, measCollection[i].idB));
  }

  ///
  /// Setup Cost Terms
  ///

  // steam cost terms
  std::vector<steam::CostTerm::Ptr> costTerms;

  // Setup shared noise and loss functions
  steam::NoiseModel::Ptr sharedNoiseModel(new steam::NoiseModel(measCollection[0].sqrtInformation, steam::NoiseModel::SQRT_INFORMATION));
  steam::L2LossFunc::Ptr sharedLossFunc(new steam::L2LossFunc());

  // Turn measurements into cost terms
  for (unsigned int i = 0; i < measCollection.size(); i++)
  {
    // Get first referenced state variable
    steam::se3::TransformStateVar::Ptr& stateVarA = poses_k_0[measCollection[i].idA];

    // Get second referenced state variable
    steam::se3::TransformStateVar::Ptr& stateVarB = poses_k_0[measCollection[i].idB];

    // Get transform measurement
    lgmath::se3::Transformation& meas_T_BA = measCollection[i].T_BA;

    // Construct error function
    steam::TransformErrorEval::Ptr errorfunc(new steam::TransformErrorEval(meas_T_BA, stateVarB, stateVarA));

    // Create cost term and add to problem
    steam::CostTerm::Ptr cost(new steam::CostTerm(errorfunc, sharedNoiseModel, sharedLossFunc));
    costTerms.push_back(cost);
  }

  ///
  /// Make Optimization Problem
  ///

  // Initialize problem
  steam::OptimizationProblem problem;

  // Add state variables
  for (unsigned int i = 1; i < poses_k_0.size(); i++)
  {
    problem.addStateVariable(poses_k_0[i]);
  }

  // Add cost terms
  for (unsigned int i = 0; i < costTerms.size(); i++)
  {
    problem.addCostTerm(costTerms[i]);
  }

  ///
  /// Setup Solver and Optimize
  ///

  // Initialize parameters (enable verbose mode)
  steam::VanillaGaussNewtonSolver::Params params;
  params.verbose = true;

  // Make solver
  steam::VanillaGaussNewtonSolver solver(&problem, params);

  // Optimize
  solver.optimize();

  return 0;
}

