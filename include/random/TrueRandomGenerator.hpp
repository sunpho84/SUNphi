#ifndef _TRUERANDOMGENERATOR_HPP
#define _TRUERANDOMGENERATOR_HPP

/// \file TrueRandomGenerator.hpp
///
/// \brief Generate random numbers according to the true random device
///
/// This random generator should be used only once, to init the master
/// random generator, when a "true" (non reproducible) random sequence
/// is needed.

#include <random>

#include <system/Mpi.hpp>
#include <utility/SingleInstance.hpp>

namespace SUNphi
{
  /// Class which enables true random numbers
  ///
  /// The random numbers are always generated on all nodes, but only
  /// the value on the given node is returned
  class TrueRandomGenerator
    : public std::random_device,
      SingleInstance<TrueRandomGenerator>
  {
    /// Rank of which the result is returning
    int returningRank;
    
  public:
    
    /// Type returned
    using ResultType=
      std::random_device::result_type;
    
    /// Creates specifying the rank to be used for returning
    TrueRandomGenerator(int returningRank=Mpi::MASTER_RANK)
      : returningRank(returningRank)
    {
    }
    
    /// Returns the inner generated value
    auto operator()()
    {
      /// Draw the value on every node
      ResultType val=
	std::random_device::operator()();
      
      // Broadcast from the preferred node
      if(returningRank!=Mpi::ALL_RANKS)
	mpi.broadcast(val,returningRank);
      
      return
	val;
    }
  };
  
  extern TrueRandomGenerator trueRandomGenerator;
}

#endif
