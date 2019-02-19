#ifndef _TRUERANDOM_HPP
#define _TRUERANDOM_HPP

/// \file TrueRandom.hpp
///
/// \brief Generate random numbers according to the true random device

#include <random>

#include <system/Mpi.hpp>

namespace SUNphi
{
  /// Class which enables true random numbers
  ///
  /// The random numbers are always generated on all nodes, but only
  /// the value on the given node is returned
  class TrueRandom
    : public std::random_device
  {
    /// Rank of which the result is returning
    int returningRank;
    
  public:
    
    /// Creates specifying the rank to be used for returning
    TrueRandom(int returningRank=Mpi::MASTER_RANK)
      : returningRank(returningRank)
    {
    }
    
    /// Returns the inner generated value
    auto operator()()
    {
      return
	mpi.broadcast(static_cast<std::random_device*>(this)->operator()(),
		      returningRank);
    }
  };
}

#endif
