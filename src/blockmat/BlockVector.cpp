//////////////////////////////////////////////////////////////////////////////////////////////
/// \file BlockVector.cpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/blockmat/BlockVector.hpp>

#include <stdexcept>

namespace steam {

  // Katarina
  //
  // Throwing exception is performance costly. Sometimes the same functionality can be achieved
  // by just returning error code, but with huge performance gain.
  // Especially if exceptions rolls-up several levels consider throwing exception only from top level.
  //

  // Katarina
  //
  // What is the use of default constructor? After default constructor is called how to
  // set indexing_ . If indexing_ is not set then BlockVector::add and BlockVector::setFromScalar
  // looks like it will fail.
  // Any test/example for the use of default constructor?

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Default constructor, vector size must still be set before using
//////////////////////////////////////////////////////////////////////////////////////////////
BlockVector::BlockVector() {}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Block size constructor
//////////////////////////////////////////////////////////////////////////////////////////////
BlockVector::BlockVector(const std::vector<unsigned int>& blkRowSizes) : indexing_(blkRowSizes) {

  data_ = Eigen::VectorXd::Zero(indexing_.scalarSize());
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Block size (with data) constructor
//////////////////////////////////////////////////////////////////////////////////////////////
BlockVector::BlockVector(const std::vector<unsigned int>& blkRowSizes, const Eigen::VectorXd& v)
 : indexing_(blkRowSizes) {

  // Katarina
  //
  // No need for this-> since there is no name ambiguity for calling setFromScalar(v) ?
  this->setFromScalar(v);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Set internal data (total size of v must match concatenated block sizes)
//////////////////////////////////////////////////////////////////////////////////////////////
void BlockVector::setFromScalar(const Eigen::VectorXd& v) {

  if (indexing_.scalarSize() != (unsigned int)v.size()) {
    std::stringstream ss; ss << "Block row size: " << indexing_.scalarSize() <<
                                " and vector size: " << v.size() << " do not match.";
    throw std::invalid_argument(ss.str());
  }

  data_ = v;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get indexing object
//////////////////////////////////////////////////////////////////////////////////////////////
const BlockDimIndexing& BlockVector::getIndexing() {
  return indexing_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Adds the vector to the block entry at index 'r', block dim must match
//////////////////////////////////////////////////////////////////////////////////////////////
void BlockVector::add(const unsigned int& r, const Eigen::VectorXd& v) {

  if (r >= indexing_.numEntries()) {
    throw std::invalid_argument("Requested row index is out of bounds.");
  }

  if (v.rows() != (int)indexing_.blkSizeAt(r)) {
    throw std::invalid_argument("Block row size and vector size do not match.");
  }

  data_.block(indexing_.cumSumAt(r),0,indexing_.blkSizeAt(r),1) += v;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Return block vector at index 'r'
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXd BlockVector::at(const unsigned int& r) {

  if (r >= indexing_.numEntries()) {
    throw std::invalid_argument("Requested row index is out of bounds.");
  }

  return data_.block(indexing_.cumSumAt(r),0,indexing_.blkSizeAt(r),1);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Return block vector at index 'r'
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Map<Eigen::VectorXd> BlockVector::mapAt(const unsigned int& r) {

  if (r >= indexing_.numEntries()) {
    throw std::invalid_argument("Requested row index is out of bounds.");
  }

  return Eigen::Map<Eigen::VectorXd>(&data_[indexing_.cumSumAt(r)],indexing_.blkSizeAt(r),1);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Convert to Eigen dense vector format
//////////////////////////////////////////////////////////////////////////////////////////////
const Eigen::VectorXd& BlockVector::toEigen() {
  return data_;
}

} // steam
