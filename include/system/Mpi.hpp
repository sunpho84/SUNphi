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
    void init()
    {
      /// Returned value of MPI
      MPI_CRASH_ON_ERROR(MPI_Init(nullptr,nullptr),"Error initializing MPI");
    }
    
    /// Check initialization flag
    int isInitialized()
    {
      /// Initialization flag
      int res;
      MPI_CRASH_ON_ERROR(MPI_Initialized(&res),"Checking MPI initialization");
      
      return res;
    }
    
    /// Finalize MPI
    void finalize()
    {
      MPI_CRASH_ON_ERROR(MPI_Finalize(),"Finalizing MPI");
    }
    
    /// Get current rank
    int getRank()
    {
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_rank(MPI_COMM_WORLD,&res),"Getting current rank");
      
      return res;
    }
    
    /// Cached value of current rank
    int rank()
    {
      /// Stored value
      static int _rank=
	getRank();
      
      return _rank;
    }
    
    /// Get the total number of ranks
    int getNRanks()
    {
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_size(MPI_COMM_WORLD,&res),"Getting total number of ranks");
      
      return res;
    }
    
    /// Cached value of total number of ranks
    int nRanks()
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
      /// Result
      T out;
      
      MPI_CRASH_ON_ERROR(MPI_Allreduce(&in,&out,1,mpiType<T>(),MPI_SUM,MPI_COMM_WORLD),"Reducing among all processes");
      
      return out;
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

#undef MPI_CRASH_ON_ERROR

#endif
