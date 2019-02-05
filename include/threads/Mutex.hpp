#ifndef _MUTEX_HPP
#define _MUTEX_HPP

/// \file Barrier.hpp
///
/// \brief Define the threads barrier

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <cstring>

#include <threads/Thread.hpp>
#include <system/Debug.hpp>

namespace SUNphi
{
#ifdef USE_THREADS
  
  /// Class to lock a mutex for the object scope
  class Mutex
  {
    /// Internal mutex
    pthread_mutex_t mutex PTHREAD_MUTEX_INITIALIZER;
    
  public:
    
    /// Lock the mutex
    void lock()
    {
      if(pthread_mutex_lock(&mutex))
	CRASH("Error locking,",strerror(errno));
    }
    
    /// Unlock the mutex
    void unlock()
    {
      if(pthread_mutex_unlock(&mutex))
	CRASH("Error unlocking,",strerror(errno));
    }
  };
  
  /// Keep a mutex locked for the duration of the object
  class ScopeMutexLocker
  {
    /// Reference to the mutex
    Mutex& mutex;
    
  public:
    
    /// Create, store the reference and lock
    ScopeMutexLocker(Mutex& mutex) ///< Mutex to be kept locked
      : mutex(mutex)
    {
      mutex.lock();
    }
    
    /// Unlock and destroy
    ~ScopeMutexLocker()
    {
      mutex.unlock();
    }
  };

#else
  
  /// Dummy mutex
  struct Mutex
  {
    /// Lock the mutex
    void lock()
    {
    }
    
    /// Unlock the mutex
    void unlock()
    {
    }
  };
  
  /// Dummy set a scope mutex locker
#define THREADS_SCOPE_SEQUENTIAL()
  
  /// Dummy scope mutex locker
  struct ScopeMutexLocker
  {
  };

#endif
}

#endif
