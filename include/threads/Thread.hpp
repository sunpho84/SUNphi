#ifndef _THREAD_HPP
#define _THREAD_HPP

/// \file Thread.hpp
///
/// \brief Basic functionalities of threads

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <pthread.h>
#include <thread>

/// States that we want to debug threads
[[ maybe_unused ]]
constexpr bool DEBUG_THREADS=
  true;
  
namespace SUNphi
{
  /// Thread id of master thread
  [[ maybe_unused ]]
  constexpr int masterThreadId=
    0;
  
  /// Makes all thread print for current scope
#ifdef USE_THREADS
 #define ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(LOGGER)		\
  SET_FOR_CURRENT_SCOPE(LOGGER_ALL_THREADS_PRINT,LOGGER.onlyMasterThreadPrint,false)
#else
  #define ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(LOGGER)
#endif
}

#endif
