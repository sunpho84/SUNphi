#ifndef _MPI_HPP
#define _MPI_HPP

/// \file Mpi.hpp
///
/// \brief Incapsulate all functionalities of MPI into a more
/// convenient form
///
/// Automatically initialize MPI with the inclusion of the library,
/// unless the macro \c SUNPHI_DO_NOT_INITIALIZE_MPI is set before
/// including SUNphi.hpp

#include <mpi.h>

#include <utility/SingleInstance.hpp>

namespace SUNphi
{
  /// Crash on MPI error, providing a meaningful error
#define MPI_CRASH_ON_ERROR(...)			\
  Mpi::CrashOnError(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)
  
  /// Class providing all functionalities of MPI
  class Mpi :
    public SingleInstance<Mpi>
  {
    
    PROVIDE_STATIC_MEMBER_WITH_INITIALIZATOR(int,_rank,0,Rank of current MPI process);
    
  public:
    
    /// Decrypt the returned value of an MPI call
    template <typename...Args>
    static void CrashOnError(const int line,
			     const char *file,
			     const char *function,
			     const int rc,
			     Args&&... args);
    
    /// Initialize MPI
    static void init()
    {
      /// Returned value of MPI
      MPI_CRASH_ON_ERROR(MPI_Init(nullptr,nullptr),"Error initializing MPI");
    }
    
    /// Check initialization flag
    static int isInitialized()
    {
      /// Initialization flag
      int res;
      MPI_CRASH_ON_ERROR(MPI_Initialized(&res),"Checking MPI initialization");
      
      return res;
    }
    
    /// Finalize MPI
    static void finalize()
    {
      MPI_CRASH_ON_ERROR(MPI_Finalize(),"Finalizing MPI");
    }
    
    /// Get current rank
    static int currRank()
    {
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_rank(MPI_COMM_WORLD,&res),"Getting current rank");
      
      return res;
    }
    
    /// Constructor, initializing MPI and filling rank
    Mpi()
    {
#ifndef SUNPHI_DO_NOT_INITIALIZE_MPI
      init();
#endif
      
      _rank()=
	currRank();
    }
    
    /// Hashed value of current rank
    static const int& rank()
    {
      return _rank();
    }
    
    /// Destructor
    ~Mpi()
    {
      finalize();
    }
  };
}

#undef MPI_CRASH_ON_ERROR

#endif
