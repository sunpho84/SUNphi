#ifndef _FAKEPOOL_HPP
#define _FAKEPOOL_HPP

/// \file FakeFool.hpp
///
/// \brief Implement a fake pool, to be used when threads are disabled

namespace SUNphi
{
#define ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(LOGGER)
  
#define ALLOWS_ALL_RANKS_TO_PRINT_FOR_THIS_SCOPE(LOGGER)
  
  /// Contains a fake thread pool
  class FakePool
  {
    /// Wrapper for the pthread barrier functionality
    ///
    /// Low level barrier not meant to be called explictly
    class Barrier
    {
      
    public:
      
      ///Build the barrier for \c nThreads threads
      Barrier(const int& nThreads) ///< Number of threads for which the barrier is defined
      {
      }
      
      /// Destroys the barrier
      ~Barrier()
      {
      }
      
      /// Synchronize, without checking the name of the barrier
      void sync()
      {
      }
      
      /// Synchronize checking the name of the barrier
      void sync(const char* barrName, ///< Name of the barrier
		const int& threadId)  ///< Id of the thread used coming to check
      {
      }
    };
    
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
    
    /// Stop the pool
    void doNotworkAnymore()
    {
    }
    
    /// Empty the thread pool
    void empty()
    {
    }
    
  public:
    
    /// Get the thread of the current thread
    int getThreadId()
      const
    {
      return
	0;
    }
    
    /// Class to lock a mutex for the object scope
    class Mutex
    {
    public:
      
      /// Lock the mutex
      void lock()
      {
      }
      
      /// Unlock the mutex
      void unlock()
      {
      }
    };
    
    /// Keep a mutex locked for the duration of the object
    class ScopeMutexLocker
    {
    };
    
  private:
    
  public:
    
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
    
    /// Start the work for the other threads
    void tellThePoolWorkIsAssigned(const int& threadId) ///< Thread id
    {
    }
    
    /// Tell the master that the thread is created and ready to swim
    void tellTheMasterThreadIsCreated(const int& threadId) ///< Thread id
    {
    }
    
    /// Waiting for threads are created and ready to swim
    void waitPoolToBeFilled(const int& threadId) ///< Thread id
    {
    }
    
    /// Waiting for work to be done means to synchronize with the master
    void waitForWorkToBeAssigned(const int& threadId) ///< Thread id
    {
    }
    
    /// Stop the pool from working
    void tellThePoolNotToWorkAnyLonger(const int& threadId) ///< Thread id
    {
    }
    
    /// Waiting for work to be done means to synchronize with the master
    void tellTheMasterWorkIsFinished(const int& threadId) ///< Thread id
    {
    }
    
    /// Wait that the work assigned to the pool is finished
    void waitForPoolToFinishAssignedWork(const int& threadId) ///< Thread id
    {
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
    
    /// Split a loop into \c nTrheads chunks, giving each chunk as a work for a corresponding thread
    template <typename Size,           // Type for the range of the loop
	      typename F>              // Type for the work function
    void loopSplit(const Size& beg,  ///< Beginning of the loop
		   const Size& end,  ///< End of the loop
		   F f)              ///< Function to be called, accepting two integers: the first is the thread id, the second the loop argument
    {
      for(Size i=beg;i<end;i++)
	f(0,i);
    }
    
    /// Constructor starting the thread pool with a given number of threads
    FakePool(int nThreads=1)
    {
    }
    
    /// Destructor emptying the pool
    ~FakePool()
    {
    }
  };
  
  using ThreadPool=
    FakePool;
  
  /// Global threads
  extern ThreadPool threads;
}

#endif
