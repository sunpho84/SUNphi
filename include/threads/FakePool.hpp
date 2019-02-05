#ifndef _FAKEPOOL_HPP
#define _FAKEPOOL_HPP

/// \file FakePool.hpp
///
/// \brief Implement a fake pool, to be used when threads are disabled

namespace SUNphi
{
  /// Allows all threads to print: this is implicit
#define ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(LOGGER)
  
  /// Contains a fake thread pool
  class FakePool
  {
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
    
  public:
    
    /// Assert that only the pool is accessing
    void assertPoolOnly(const int& threadId) ///< Calling thread
      const
    {
    }
    
    /// Assert that only the master thread is accessing
    void assertMasterOnly(const int& threadId) ///< Calling thread
      const
    {
    }
    
    /// Get the thread of the current thread
    int getThreadId()
      const
    {
      return
	0;
    }
    
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
    
    /// Dummy mutex scope locker
    struct ScopeMutexLocker
    {
    };
    
    /// Puts a scope mutex locker making the scope sequential
#define THREADS_SCOPE_SEQUENTIAL()
    
    /// Lock the internal mutex
    void mutexLock()
    {
    }
    
    /// Unlock the mutex
    void mutexUnlock()
    {
    }
    
    /// Compares the thread tag with the master one
    bool isMasterThread()
      const
    {
      return
	true;
    }
    
    /// Gets the number of allocated threads
    int nActiveThreads()
      const
    {
      return
	1;
    }
    
    
    /// Return whether the pool is waiting for work
    bool getIfWaitingForWork()
      const
    {
      return
	false;
    }
    
    /// Gives to all threads some work to be done
    ///
    /// The object \c f must be callable, returning void and getting
    /// an integer as a parameter, representing the thread id
    template <typename F>
    void workOn(F f) ///< Function embedding the work
    {
      f(0);
    }
    
    /// Perform a loop
    template <typename Size,           // Type for the range of the loop
	      typename F>              // Type for the work function
    void loopSplit(const Size& beg,  ///< Beginning of the loop
		   const Size& end,  ///< End of the loop
		   F f)              ///< Function to be called, accepting two integers: the first is the thread id, which will always be 0, the second the loop argument
    {
      for(Size i=beg;i<end;i++)
	f(0,i);
    }
    
    /// Dummy constructor
    FakePool(int nThreads=1)
    {
    }
  };
  
  /// Wraps the \c FakePool
  using ThreadPool=
    FakePool;
  
  /// Global threads
  extern ThreadPool threads;
}

#endif
