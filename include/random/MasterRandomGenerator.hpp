#ifndef _MASTERRANDOMGENERATOR_HPP
#define _MASTERRANDOMGENERATOR_HPP

/// \file MasterRandomGenerator.hpp
///
/// \brief Master random generator, used to start streams
///
/// This random generator is expected to be used few times, so we
/// don't care using a slow one not supporting big skip
///
/// \todo Implement the skipping version

#include <random/TrueRandomGenerator.hpp>

namespace SUNphi
{
  /// Inner generator used
  using _MasterRandomGenerator=
    std::mt19937_64;
  
  /// Master random generator
  class MasterRandomGenerator :
    _MasterRandomGenerator
  {
    /// Number of seeds needed to initialize the state
    static constexpr int nSeeds=
      sizeof(_MasterRandomGenerator::state_size)/sizeof(uint32_t);
    
    /// Seed state
    std::vector<uint32_t> _seeds{nSeeds};
    
    /// Gives a constant access to the seeds
    const std::vector<uint32_t>& seeds=
      _seeds;
    
    /// Base random generator
    using Base=
      _MasterRandomGenerator;
    
  public:
    
    MasterRandomGenerator()
    {
      std::generate(_seeds.begin(),_seeds.end(),std::ref(trueRandomGenerator));
      std::seed_seq dseeds(seeds.begin(),seeds.end());
    }
  };
}

#endif
