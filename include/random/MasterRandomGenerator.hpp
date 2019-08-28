#ifndef _MASTERRANDOMGENERATOR_HPP
#define _MASTERRANDOMGENERATOR_HPP

/// \file MasterRandomGenerator.hpp
///
/// \brief Master random generator, used to start streams
///
/// This random generator is expected to be used few times, so we
/// don't care using a slow one not supporting big skip

#include <random/TrueRandomGenerator.hpp>
#include <Serialize.hpp>

namespace SUNphi
{
  /// Inner generator used
  using _MasterRandomGenerator=
    std::mt19937_64;
  
  /// Master random generator
  class MasterRandomGenerator :
    public SerializableClass<MasterRandomGenerator>,
    public _MasterRandomGenerator
  {
    /// Number of seeds needed to initialize the state
    static constexpr int nSeeds=
      _MasterRandomGenerator::state_size;
    
    /// Seed state
    SERIALIZABLE_SCALAR_WITH_TAG(std::vector<uint64_t>,_seeds,"Seeds",nSeeds);
    
    /// Constant access to the seeds
    const std::vector<uint64_t>& seeds=
      _seeds;
    
    /// Base random generator
    using Base=
      _MasterRandomGenerator;
    
  public:
    
    LIST_SERIALIZABLE_MEMBERS(_seeds);
    
    /// Reset the number generator to the initial state
    void reset()
    {
      // Prepares a seed sequence to initialize
      std::seed_seq dseeds(seeds.begin(),seeds.end());
      
      this->seed(dseeds);
    }
    
    /// Skip n elements
    void skip(const int n)
    {
      for(int i=0;i<n;i++)
	(*this)();
    }
    
    /// Fills _seeds using the true random generator
    MasterRandomGenerator()
    {
      std::generate(_seeds().begin(),_seeds().end(),std::ref(trueRandomGenerator));
      
      reset();
    }
  };
}

#endif
