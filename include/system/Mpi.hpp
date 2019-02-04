#ifndef _MPI_HPP
#define _MPI_HPP

/// \file Mpi.hpp
///
/// \brief Incapsulate all functionalities of MPI into a more
/// convenient form

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#ifdef USE_MPI
 #include <mpi.h>
#endif

#include <ios/MinimalLogger.hpp>
#include <system/Threads.hpp>
#include <system/Timer.hpp>
#include <utility/SingleInstance.hpp>

namespace SUNphi
{
  /// Provides the \c MPI_Datatype of an any unknown type
  template <typename T>
  inline MPI_Datatype mpiType()
  {
    return
      nullptr;
  }
  
  /// Provides the \c MPI_Datatype of a given type
#define PROVIDE_MPI_DATATYPE(MPI_TYPE,TYPE)		\
  template <>						\
  inline MPI_Datatype mpiType<TYPE>()			\
  {							\
    return						\
      MPI_TYPE;						\
  }							\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  PROVIDE_MPI_DATATYPE(MPI_CHAR,char);
  
  PROVIDE_MPI_DATATYPE(MPI_INT,int);
  
  PROVIDE_MPI_DATATYPE(MPI_DOUBLE,double);
  
  /// Class wrapping all MPI functionalities
  class Mpi : public SingleInstance<Mpi>
  {
#ifdef USE_MPI
    /// Crash on MPI error, providing a meaningful error
#define MPI_CRASH_ON_ERROR(...)						\
    Mpi::crashOnError(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)
#endif
    
    /// Decrypt the returned value of an MPI call
    ///
    /// Returns the value of \c rc
    template <typename...Args>
    int crashOnError(const int line,        ///< Line of file where the error needs to be checked
		     const char *file,      ///< File where the error must be checked
		     const char *function,  ///< Function where the error was possibly raised
		     const int rc,          ///< Exit condition of the called routine
		     Args&&... args)        ///< Other arguments
      const
    {
#ifdef USE_MPI
      
      if(rc!=MPI_SUCCESS and rank()==0)
	{
	  /// Length of the error message
	  int len;
	  
	  /// Error message
	  char err[MPI_MAX_ERROR_STRING];
	  MPI_Error_string(rc,err,&len);
	  
	  internalCrash(line,file,args...,", raised error",rc,":",err);
	}
      
#endif
      
      return
	rc;
    }
    
  public:
    
    /// Id of master rank
    static constexpr int MASTER_RANK=
      0;
    
    /// Placeholder for all ranks
    [[ maybe_unused ]]
    static constexpr int ALL_RANKS=
      -1;
    
    /// Initialize MPI
    ///
    /// Implemented in cpp
    Mpi();
    
    /// Check initialization flag
    bool isInitialized()
      const
    {
      
#ifdef USE_MPI
      
      /// Initialization flag
      int res;
      MPI_CRASH_ON_ERROR(MPI_Initialized(&res),"Checking MPI initialization");
      
      return
	res;
      
#else
      
      return
	true;
      
#endif
    }
    
    /// Finalize MPI
    ///
    /// Implemented in cpp
    ~Mpi();
    
    /// Get current rank calling explicitly MPI
    int getRank()
      const
    {
      
#ifdef USE_MPI
      
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_rank(MPI_COMM_WORLD,&res),"Getting current rank");
      
      return
	res;
      
#else
      
      return
	0;
      
#endif
      
    }
    
    /// Cached value of current rank
    int rank()
      const
    {
      /// Stored value
      static int _rank=
	getRank();
      
      return
	_rank;
    }
    
    /// Get the total number of ranks, calling explicitly MPI
    int getNRanks()
      const
    {
      
#ifdef USE_MPI
      
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_size(MPI_COMM_WORLD,&res),"Getting total number of ranks");
      
      return
	res;
      
#else
      
      return
	1;
	
#endif
      
    }
    
    /// Check if this is the master rank
    bool isMasterRank()
      const
    {
      /// Store the result
      static bool is=
	(rank()==MASTER_RANK);
      
      return
	is;
    }
    
    /// Cached value of total number of ranks
    int nRanks()
      const
    {
      /// Stored value
      static int _nRanks=
	getNRanks();
      
      return
	_nRanks;
    }
    
    /// Reduces among all MPI process
    template <typename T>
    T allReduce(const T& in)
      const
    {
      
#ifdef USE_MPI
      
      /// Result
      T out;
      
      minimalLogger(runLog,"%p %d",&out,rank());
      
      MPI_CRASH_ON_ERROR(MPI_Allreduce(&in,&out,1,mpiType<T>(),MPI_SUM,MPI_COMM_WORLD),"Reducing among all processes");
      
      return
	out;
      
#else
      
      return
	in;
      
#endif
      
    }
  };
  
  /// Gloabl MPI
  extern Mpi mpi;
}

#endif
