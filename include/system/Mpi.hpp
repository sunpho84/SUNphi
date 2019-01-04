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
  
  /// Class providing all functionalities of MPI
  class Mpi :
    public SingleInstance<Mpi>
  {
    
    PROVIDE_STATIC_MEMBER_WITH_INITIALIZATOR(int,_rank,0,Rank of current MPI process);
    
    PROVIDE_STATIC_MEMBER_WITH_INITIALIZATOR(int,_nRanks,0,Total number of ranks);
    
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
    static int getRank()
    {
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_rank(MPI_COMM_WORLD,&res),"Getting current rank");
      
      return res;
    }
    
    /// Get the total number of ranks
    static int getNRanks()
    {
      /// Returned value
      int res;
      MPI_CRASH_ON_ERROR(MPI_Comm_size(MPI_COMM_WORLD,&res),"Getting total number of ranks");
      
      return res;
    }
    
    /// Constructor, initializing MPI and filling rank
    Mpi()
    {
#ifndef SUNPHI_DO_NOT_INITIALIZE_MPI
      init();
#endif
      
      _rank()=
	getRank();
      
      _nRanks()=
	getNRanks();
    }
    
    /// Hashed value of current rank
    static const int& rank()
    {
      return _rank();
    }
    
    /// Hashed value of nRanks
    static const int& nRanks()
    {
      return _nRanks();
    }
    
    /// Destructor
    ~Mpi()
    {
      finalize();
    }
    
    /// Reduces among all MPI process
    template <typename T>
    static T allReduce(const T& in)
    {
      /// Result
      T out;
      
      MPI_CRASH_ON_ERROR(MPI_Allreduce(&in,&out,1,mpiType<T>(),MPI_SUM,MPI_COMM_WORLD),"Reducing among all processes");
      
      return out;
    }
  };
}

#undef MPI_CRASH_ON_ERROR

#endif
