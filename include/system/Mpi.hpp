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

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#ifdef USE_MPI
 #include <mpi.h>
#endif

#include <utility/SingleInstance.hpp>

namespace SUNphi
{
 #ifdef USE_MPI
  
  /// Provides link from a type to the mathcing \c MPI_Datatype
  ///
  /// Useful to allow template usage of MPI
#define PROVIDE_MPI_DATATYPE(MPI_TYPE,TYPE)		\
  /*! \c MPI_Datatype corresponding to TYPE */		\
  template <>						\
  MPI_Datatype mpiType<TYPE>()				\
  {							\
    return MPI_TYPE;					\
  }
  
  /// MPI datatpe corresponding to not-provided type
  template <typename T>
  MPI_Datatype mpiType()
  {
    return nullptr;
  }
  
  PROVIDE_MPI_DATATYPE(MPI_CHAR,char);
  
  PROVIDE_MPI_DATATYPE(MPI_INT,int);
  
  PROVIDE_MPI_DATATYPE(MPI_DOUBLE,double);
  
  /// Crash on MPI error, providing a meaningful error
#define MPI_CRASH_ON_ERROR(...)			\
  Mpi::CrashOnError(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)
  
#endif
  
  namespace Mpi
  {
    /// Decrypt the returned value of an MPI call
    ///
    /// Returns the value of \c rc
    template <typename...Args>
    int CrashOnError(const int line,
		     const char *file,
		     const char *function,
		     const int rc,
		     Args&&... args);
    
    /// Initialize MPI
    inline void init()
    {
#ifdef USE_MPI
      MPI_CRASH_ON_ERROR(MPI_Init(nullptr,nullptr),"Error initializing MPI");
#endif
    }
    
    /// Check initialization flag
    inline int isInitialized()
    {
      
#ifdef USE_MPI
      
      /// Initialization flag
      int res;
      MPI_CRASH_ON_ERROR(MPI_Initialized(&res),"Checking MPI initialization");
      
      return res;
      
#else
      
      return 1;
      
#endif
      
    }
    
    /// Finalize MPI
    inline void finalize()
    {
      
#ifdef USE_MPI
      
      MPI_CRASH_ON_ERROR(MPI_Finalize(),"Finalizing MPI");
      
#endif
      
    }
    
    /// Get current rank
    inline int getRank()
    {
      
#ifdef USE_MPI
      
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_rank(MPI_COMM_WORLD,&res),"Getting current rank");
      
      return res;
      
#else
      
      return 0;
      
#endif
      
    }
    
    /// Cached value of current rank
    inline int rank()
    {
      /// Stored value
      static int _rank=
	getRank();
      
      return _rank;
    }
    
    /// Get the total number of ranks
    inline int getNRanks()
    {
      
#ifdef USE_MPI
      
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_size(MPI_COMM_WORLD,&res),"Getting total number of ranks");
      
      return res;
      
#else
      
      return 1;
	
#endif
      
    }
    
    /// Cached value of total number of ranks
    inline int nRanks()
    {
      /// Stored value
      static int _nRanks=
	getNRanks();
      
      return _nRanks;
    }
    
    /// Reduces among all MPI process
    template <typename T>
    T allReduce(const T& in)
    {
      
#ifdef USE_MPI
      
      /// Result
      T out;
      
      MPI_CRASH_ON_ERROR(MPI_Allreduce(&in,&out,1,mpiType<T>(),MPI_SUM,MPI_COMM_WORLD),"Reducing among all processes");
      
      return out;
      
#else
      
      return in;
      
#endif
      
    }
    
    /// Class used to make MPI alive
    class MakeAlive :
      public SingleInstance<MakeAlive>
    {
  public:
    
      /// Constructor, initializing MPI
      MakeAlive()
      {
#ifndef SUNPHI_DO_NOT_INITIALIZE_MPI
	init();
#endif
      }
      
      /// Destructor
      ~MakeAlive()
      {
	finalize();
      }
    };
  }
}

#ifdef USE_MPI
 
 #undef MPI_CRASH_ON_ERROR
 
#endif

#endif
