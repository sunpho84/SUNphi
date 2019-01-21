#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file SUNphi.cpp
///
/// \brief Main file for the SUNphi library

#include <system/MpiImpl.hpp>

/// Namespace of the SUNphi library
namespace SUNphi
{
  /// Initialize the SUNphi library
  __attribute__((constructor))
  static void makeAlive()
  {
    Mpi::init();
  }
  
  /// Finalize the SUNphi library
  __attribute__((destructor))
  static void makeDead()
  {
    Mpi::finalize();
  }
}

