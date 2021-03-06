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
#include <metaprogramming/TypeTraits.hpp>
#include <system/Timer.hpp>
#include <serialize/Binarize.hpp>
#include <utility/SingleInstance.hpp>
#include <Threads.hpp>

namespace SUNphi
{
  /// Makes all thread print for current scope
#define ALLOWS_ALL_RANKS_TO_PRINT_FOR_THIS_SCOPE(LOGGER)		\
  SET_FOR_CURRENT_SCOPE(LOGGER_ALL_RANKS_PRINT,LOGGER.onlyMasterRankPrint,false)
  
#ifdef USE_MPI
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
#endif
  
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
	  
	  minimalCrash(file,line,__PRETTY_FUNCTION__,"(args ignored!), raised error %d, err: %s",rc,err);
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
    Mpi()
    {
#ifdef USE_MPI
      
      /// Takes the time
      Duration initDur;
      
      MPI_CRASH_ON_ERROR(durationOf(initDur,MPI_Init,nullptr,nullptr),"Error initializing MPI");
      
      minimalLogger(runLog,"MPI initialized in %lg s",durationInSec(initDur));
#endif
    }
    
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
    ~Mpi()
    {
    
#ifdef USE_MPI
      
      MPI_CRASH_ON_ERROR(MPI_Finalize(),"Finalizing MPI");
      
#endif
    }
    
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
    
    /// Broadcast among all MPI process
    ///
    /// This is a simple wrapper around the MPI_Bcast function
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isTriviallyCopyable<T>)>
    void broadcast(T* x,                   ///< Quantity to broadcast
		   const size_t& size,     ///< Size of the quantity to broadcast
		   int root=MASTER_RANK)   ///< Rank from which to broadcast
      const
    {
#ifdef USE_MPI
      minimalLogger(runLog,"%p %d",x,rank());
      MPI_CRASH_ON_ERROR(MPI_Bcast(x,size,MPI_CHAR,root,MPI_COMM_WORLD),"Broadcasting");
#endif
    }
    
    /// Broadcast among all MPI process
    ///
    /// Accepts trivially copyable structures
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isTriviallyCopyable<T>)>
    void broadcast(T& x,                   ///< Quantity to broadcast
		   int root=MASTER_RANK)   ///< Rank from which to broadcast
      const
    {
      broadcast(&x,sizeof(T),root);
    }
    
    /// Broadcast among all MPI process
    ///
    /// Accepts all binarizable classes
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isBinarizable<T>)>
    void broadcast(T&& val,                ///< Quantity to broadcast
		   int root=MASTER_RANK)   ///< Rank from which to broadcast
      const
    {
      
#ifdef USE_MPI
      Binarizer bin=
	val.binarize();
      
      broadcast(&*bin.begin(),bin.size(),root);
      
      val.deBinarize(bin);
#endif
    }
  };
  
  /// Gloabl MPI
  extern Mpi mpi;
}

#endif
