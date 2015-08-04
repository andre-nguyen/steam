//////////////////////////////////////////////////////////////////////////////////////////////
/// \file JacobianTreeBranchNode.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_JACOBIAN_TREE_BRANCH_NODE_HPP
#define STEAM_JACOBIAN_TREE_BRANCH_NODE_HPP

#include <vector>
#include <Eigen/Dense>
#include <steam/evaluator/jacobian/JacobianTreeNode.hpp>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Simple structure to hold Jacobian information
//////////////////////////////////////////////////////////////////////////////////////////////
class JacobianTreeBranchNode : public JacobianTreeNode
{
 public:

  /// Convenience typedefs
  typedef boost::shared_ptr<JacobianTreeBranchNode > Ptr;
  typedef boost::shared_ptr<const JacobianTreeBranchNode > ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Default constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  JacobianTreeBranchNode(unsigned int reserveNum = 4);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Add a child node to the branch
  //////////////////////////////////////////////////////////////////////////////////////////////
  //void add(const Eigen::MatrixXd& lhsJacobian, const JacobianTreeNode::ConstPtr& child);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Add a child node to the branch
  //////////////////////////////////////////////////////////////////////////////////////////////
  Eigen::MatrixXd& add(const JacobianTreeNode::ConstPtr& child);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Traverse the Jacobian tree and calculate the Jacobians
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void append(std::vector<Jacobian>* outJacobians) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Traverse the Jacobian tree and calculate the Jacobians, pre-multiplied by lhs
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void append(const Eigen::MatrixXd& lhs, std::vector<Jacobian>* outJacobians) const;

 private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Children nodes
  //////////////////////////////////////////////////////////////////////////////////////////////
  typedef std::pair<Eigen::MatrixXd, JacobianTreeNode::ConstPtr> JacobianEdge_t;
  std::vector<JacobianEdge_t> children_;

};

} // steam

#endif // STEAM_JACOBIAN_TREE_BRANCH_NODE_HPP
