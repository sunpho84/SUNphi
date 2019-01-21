#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file Aliver.cpp
///
/// \brief Starts the library and close it

#include <system/MpiImpl.hpp>

/// Namespace of the SUNphi library
namespace SUNphi
{
  int aliverHelper()
  {
    return
      0;
  }
  
  /// Initialize the SUNphi library
  __attribute__((constructor))
  void makeAlive()
  {
    Mpi::init();
  }
  
  /// Finalize the SUNphi library
  __attribute__((destructor))
  void makeDead()
  {
    Mpi::finalize();
  }
}

