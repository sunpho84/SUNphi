#ifndef _BARRIER_HPP
#define _BARRIER_HPP

/// \file Barrier.hpp
///
/// \brief Define the threads barrier

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <cstring>

#include <debug/MinimalCrash.hpp>
#include <threads/Thread.hpp>

namespace SUNphi
{
#ifdef USE_THREADS
  /// Wrapper for the pthread barrier functionality
  ///
  /// Low level barrier not meant to be called explictly
  class Barrier
  {
    /// Raw barrier
    pthread_barrier_t barrier;
    
#ifdef DEBUG_MODE
    
    /// Value used to check the barrier
    [[maybe_unused ]]
    const char* currBarrName;
    
#endif
    
    /// Raw synchronization, simply wait that all threads call this
    void rawSync()
    {
      /// Call the barrier and get the result
      const int rc=
	pthread_barrier_wait(&barrier);
      
      if(rc!=0 and rc!=PTHREAD_BARRIER_SERIAL_THREAD)
	MINIMAL_CRASH_STDLIBERR("while barrier was waiting");
    }
    
  public:
    
    /// Build the barrier for \c nThreads threads
    Barrier(const int& nThreads) ///< Number of threads for which the barrier is defined
    {
      if(pthread_barrier_init(&barrier,nullptr,nThreads)!=0)
	MINIMAL_CRASH_STDLIBERR("while barrier inited");
    }
    
    /// Destroys the barrier
    ~Barrier()
    {
      if(pthread_barrier_destroy(&barrier)!=0)
	MINIMAL_CRASH_STDLIBERR("while barrier was destroyed");
    }
    
    /// Synchronize, without checking the name of the barrier
    void sync()
    {
      rawSync();
    }
    
    /// Synchronize checking the name of the barrier
    void sync(const char* barrName, ///< Name of the barrier
	      const int& threadId)  ///< Id of the thread used coming to check
    {
      rawSync();
      
#ifdef DEBUG_MODE
      
      if(threadId==masterThreadId)
	currBarrName=
	  barrName;
      
      rawSync();
      
      if(currBarrName!=barrName)
	MINIMAL_CRASH("Thread id %d was expecting %s but encountered %s",threadId,currBarrName,barrName);
      
#endif
      
    }
  };
  
#else
  
  /// Fake barrier
  struct Barrier
  {
    /// Dummy build the barrier for \c nThreads threads
    Barrier(const int& nThreads) ///< Number of threads for which the barrier is defined
    {
    }
    
    /// Dummy synchronize checking the name of the barrier
    void sync(const char* barrName=nullptr, ///< Name of the barrier
	      const int& threadId=0)        ///< Id of the thread used coming to check
    {
    }
  };
  
#endif

}

#endif
