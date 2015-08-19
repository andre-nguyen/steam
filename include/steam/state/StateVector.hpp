//////////////////////////////////////////////////////////////////////////////////////////////
/// \file StateVector.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_STATE_VECTOR_HPP
#define STEAM_STATE_VECTOR_HPP

#include <steam/state/StateVariableBase.hpp>
#include <unordered_map>
#include <vector>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Class to contain a list of the active state variables.
///        Also keeps track of the 'block' ordering (affects visual sparsity)
//////////////////////////////////////////////////////////////////////////////////////////////
class StateVector
{
 public:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Default constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  StateVector();

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Copy constructor -- deep copy
  //////////////////////////////////////////////////////////////////////////////////////////////
  StateVector(const StateVector& other);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Assignment operator -- deep copy
  //////////////////////////////////////////////////////////////////////////////////////////////
  StateVector& operator=(const StateVector& other);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Copy the values of 'other' into 'this' (states must already align, typically
  ///        this means that one is already a deep copy of the other)
  //////////////////////////////////////////////////////////////////////////////////////////////
  void copyValues(const StateVector& other);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Add state variable
  //////////////////////////////////////////////////////////////////////////////////////////////
  void addStateVariable(const StateVariableBase::Ptr& statevar);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Check if a state variable exists in the vector
  //////////////////////////////////////////////////////////////////////////////////////////////
  bool hasStateVariable(const StateKey& key) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get a state variable using a key
  //////////////////////////////////////////////////////////////////////////////////////////////
  StateVariableBase::ConstPtr getStateVariable(const StateKey& key) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get number of state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int getNumberOfStates() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get the block index of a state
  //////////////////////////////////////////////////////////////////////////////////////////////
  int getStateBlockIndex(const StateKey& key) const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get an ordered list of the sizes of the 'block' state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<unsigned int> getStateBlockSizes() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Update the state vector
  //////////////////////////////////////////////////////////////////////////////////////////////
  void update(const Eigen::VectorXd& perturbation);

 private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Container of state-related and indexing variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  struct StateContainer
  {
    /// State
    StateVariableBase::Ptr state;

    /// Block index in active state (set to -1 if not an active variable)
    int localBlockIndex;
  };

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Main container for state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  std::unordered_map<StateID, StateContainer> states_;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Block size of the related linear system
  //////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int numBlockEntries_;

};

} // steam

#endif // STEAM_STATE_VECTOR_HPP
