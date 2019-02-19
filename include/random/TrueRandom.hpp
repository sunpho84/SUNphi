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
  class TrueRandom
    : public std::random_device
  {
  public:
    
    auto operator()()
    {
      return
	mpi.broadcast(static_cast<std::random_device*>(this)->operator()());
    }
  };
}

#endif
