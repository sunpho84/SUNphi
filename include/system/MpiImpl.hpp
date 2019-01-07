#ifndef _MPI_IMPL_HPP
#define _MPI_IMPL_HPP

/// \file MpiImpl.hpp
///
/// \brief Internal implementation of some MPI function

#include <system/Mpi.hpp>

namespace SUNphi::Mpi
{
  /// Decrypt the returned value of an MPI call
  template <typename...Args>
  int CrashOnError(const int line,        ///< Line of file where the error needs to be checked
		    const char *file,      ///< File where the error must be checked
		    const char *function,  ///< Function where the error was possibly raised
		    const int rc,          ///< Exit condition of the called routine
		    Args&&... args)        ///< Other arguments
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
    
    return rc;
  }
}

#endif
