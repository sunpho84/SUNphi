#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <ios/Logger.hpp>
#include <system/Mpi.hpp>

namespace SUNphi
{
  Mpi::Mpi()
  {
#ifdef USE_MPI
    
    /// Takes the time
    Duration initDur;
    
    MPI_CRASH_ON_ERROR(durationOf(initDur,MPI_Init,nullptr,nullptr),"Error initializing MPI");
    
    logger<<"MPI initialized in "<<durationInSec(initDur)<<" s \n";
#endif
  }
  
  Mpi::~Mpi()
  {
    
#ifdef USE_MPI
    
    MPI_CRASH_ON_ERROR(MPI_Finalize(),"Finalizing MPI");
    
#endif
    
  }
}
