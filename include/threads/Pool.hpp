#ifndef _POOL_HPP
#define _POOL_HPP

/// \file Pool.hpp
///
/// \brief Provides a thread pool
///
/// The thread pool is implemented through \c pthread library, so we
/// can start and stop the threads in every moment of the execution,
/// at variance with the static-behavior of \c Nissa library, where
/// the program would have opened a \c OpenMP parallel statement and
/// jumped inside it.
///

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <external/inplace_function.h>

#include <Tuple.hpp>
#include <containers/Vector.hpp>
#include <debug/MinimalCrash.hpp>
#include <ios/MinimalLogger.hpp>
#include <threads/Barrier.hpp>
#include <threads/Mutex.hpp>
#include <threads/Thread.hpp>

namespace SUNphi
{
#ifdef USE_THREADS
  
  /// Contains a thread pool
  class ThreadPool
  {
    /// Maximal size of the stack used for thw work
    static constexpr int MAX_POOL_FUNCTION_SIZE=
      128;
    
    /// States if the pool is waiting for work
    bool isWaitingForWork{false};
    
    /// States if the pool is filled
    bool isFilled{false};
    
    /// Thread id of master thread
    const pthread_t masterThreadTag{getThreadTag()};
    
    /// Type to encapsulate the work to be done
    using Work=
      stdext::inplace_function<void(int),MAX_POOL_FUNCTION_SIZE>;
    
    /// Work to be done in the pool
    ///
    /// This incapsulates a function returning void, and getting an
    /// integer as an argument, corresponding to the thread
    Work work;
    
    /// Incapsulate the threads
    ///
    /// At the beginning, the pool contains only the main thread, with
    /// its id. Then when is filled, the pool contains the thread
    /// identifier. This is an opaque number, which cannot serve the
    /// purpose of getting the thread progressive in the pool. This is
    /// why we define the next function
    Vector<pthread_t> pool;
    
    /// Return the thread tag
    static pthread_t getThreadTag()
    {
      return
	pthread_self();
      return
	0;
    }
    
    /// Number of threads
    int nThreads;
    
    /// Barrier used by the threads
    Barrier barrier;
    
    /// Pair of parameters containing the threadpool and the thread id
    using ThreadPars=
      Tuple<ThreadPool*,int>;
    
    /// Function called when starting a thread
    ///
    /// When called, get the thread pool and the thread id as
    /// arguments through the function parameter. This is expcted to
    /// be allocated outside through a \c new call, so it is deleted
    /// after taking reference to the pool, and checking the thread thread.
    ///
    /// All threads but the master one swim in this pool back and forth,
    /// waiting for job to be done.
    static void* threadPoolSwim(void* _ptr) ///< Initialization data
    {
      /// Cast the \c void pointer to the tuple
      ThreadPool::ThreadPars* ptr=
	static_cast<ThreadPool::ThreadPars*>(_ptr);
      
      /// Takes a reference to the parameters
      ThreadPool::ThreadPars& pars=
	*ptr;
      
      /// Takes a reference to the pool
      ThreadPool& pool=
	*get<0>(pars);
      
      /// Copy the thread id
      int threadId=
	get<1>(pars);
      
      delete ptr;
      
      //runLog<<"entering the pool";
      
      /// Work until asked to empty
      bool keepSwimming=
	pool.isFilled;
      
      pool.tellTheMasterThreadIsCreated(threadId);
      
      while(keepSwimming)
	{
	  pool.waitForWorkToBeAssigned(threadId);
	  
	  keepSwimming=
	    pool.isFilled;
	  
	  minimalLogger(runLog," keep swimming: %d",keepSwimming);
	  
	  if(keepSwimming)
	    {
	      pool.work(threadId);
	      
	      pool.tellTheMasterWorkIsFinished(threadId);
	    }
	}
      
      minimalLogger(runLog,"exiting the pool");
      
      return
	nullptr;
    }
    
    /// Fill the pool with the number of thread assigned
    void fill(const pthread_attr_t* attr=nullptr); ///< Possible attributes of the threads
    
    /// Stop the pool
    void doNotworkAnymore()
    {
      // Mark that the pool is not waiting any more for work
      isWaitingForWork=
	false;
      
      // Temporary name to force the other threads go out of the pool
      barrier.sync();
    }
    
    /// Empty the thread pool
    void empty()
    {
      // Check that the pool is not empty
      if(not isFilled)
	MINIMAL_CRASH("Cannot empty an empty pool!");
      
      // Mark that the pool is not filled
      isFilled=
	false;
      
      /// Stop the pool from working
      tellThePoolNotToWorkAnyLonger(masterThreadId);
      
      for(int threadId=1;threadId<nThreads;threadId++)
	{
	  if(pthread_join(pool[threadId],nullptr)!=0)
	    MINIMAL_CRASH_STDLIBERR("joining threads");
	  
	  minimalLogger(runLog,"Thread of id %d destroyed",(int)threadId);
	}
      
      // Resize down the pool
      pool.resize(1);
    }
    
  public:
    
    /// Assert that only the pool is accessing
    void assertPoolOnly(const int& threadId) ///< Calling thread
      const
    {
      if(threadId==masterThreadId)
	MINIMAL_CRASH("Only pool threads are allowed");
    }
    
    /// Assert that only the master thread is accessing
    void assertMasterOnly(const int& threadId) ///< Calling thread
      const
    {
      if(threadId!=masterThreadId)
	MINIMAL_CRASH("Only master thread is allowed, but thread",threadId,"is trying to act");
    }
    
    /// Get the thread of the current thread
    int getThreadId()
      const
    {
      /// Current pthread
      const pthread_t threadTag=
	getThreadTag();
      
      /// Position in the pool
      int threadId=
	pool.findFirst(threadTag);
      
      // Check that the thread is found
      if(threadId==nActiveThreads())
	{
	  fprintf(stdout,"%d %d\n",threadId,nActiveThreads());
	  for(auto & p : pool)
	    fprintf(stdout,"%d\n",(int)p);
	  MINIMAL_CRASH("Unable to find thread with tag %d",threadTag);
	}
      
      return
	threadId;
    }
    
    
  private:
    
    /// Global mutex
    Mutex mutex;
    
  public:
    
    /// Puts a scope mutex locker making the scope sequential
#define THREADS_SCOPE_SEQUENTIAL()				\
    ScopeMutexLocker sequentializer ## __LINE__ (mutex);
    
    /// Lock the internal mutex
    void mutexLock()
    {
      mutex.lock();
    }
    
    /// Unlock the mutex
    void mutexUnlock()
    {
      mutex.unlock();
    }
    
    /// Compares the thread tag with the master one
    bool isMasterThread()
      const
    {
      return
	getThreadTag()==masterThreadTag;
    }
    
    /// Gets the number of allocated threads
    int nActiveThreads()
      const
    {
      return
	pool.size();
    }
    
    /// Tag to mark that assignment has been finished
    static constexpr char workAssignmentTag[]=
      "WorkAssOrNoMoreWork";
    
    /// Tag to mark that no more work has to do
    static constexpr auto& workNoMoreTag=
      workAssignmentTag;
    
    /// Start the work for the other threads
    void tellThePoolWorkIsAssigned(const int& threadId) ///< Thread id
    {
      assertMasterOnly(threadId);
      
      minimalLogger(runLog,"Telling the pool that work has been assigned (tag: %s)",workAssignmentTag);
      
      // Mark down that the pool is not waiting for work
      isWaitingForWork=
	false;
      
      // The master signals to the pool to start work by synchronizing with it
      barrier.sync(workAssignmentTag,threadId);
    }
    
    /// Tag to mark that the thread is ready to swim
    static constexpr char threadHasBeenCreated[]=
      "ThreadHasBeenCreated";
    
    /// Tell the master that the thread is created and ready to swim
    void tellTheMasterThreadIsCreated(const int& threadId) ///< Thread id
    {
      assertPoolOnly(threadId);
      
      minimalLogger(runLog,"Telling that thread has been created and is ready to swim (tag: %s)",threadHasBeenCreated);
      
      // The thread signals to the master that has been created and ready to swim
      barrier.sync(threadHasBeenCreated,threadId);
    }
    
    /// Waiting for threads are created and ready to swim
    void waitPoolToBeFilled(const int& threadId) ///< Thread id
    {
      assertMasterOnly(threadId);
      
      minimalLogger(runLog,"waiting for threads in the pool to be ready to ready to swim (tag: %s)",threadHasBeenCreated);
      
      // The master wait that the threads have been created by syncing with them
      barrier.sync(threadHasBeenCreated,threadId);
    }
    
    /// Waiting for work to be done means to synchronize with the master
    void waitForWorkToBeAssigned(const int& threadId) ///< Thread id
    {
      assertPoolOnly(threadId);
      
      // This printing is messing up, because is occurring in the pool
      // where the thread is expected to be already waiting for work,
      // and is not locking the logger correctly
      //minimalLogger(runLog,"waiting in the pool for work to be assigned (tag %s)",workAssignmentTag);
      
      barrier.sync(workAssignmentTag,threadId);
    }
    
    /// Stop the pool from working
    void tellThePoolNotToWorkAnyLonger(const int& threadId) ///< Thread id
    {
      assertMasterOnly(threadId);
      
      if(not isWaitingForWork)
	MINIMAL_CRASH("We cannot stop a working pool");
      
      minimalLogger(runLog,"Telling the pool not to work any longer (tag: %s)",workNoMoreTag);
      
      // Mark down that the pool is waiting for work
      isWaitingForWork=
	false;
      
      // The master signals to the pool that he is waiting for the
      // pool to finish the work
      barrier.sync(workNoMoreTag,threadId);
    }
    
    /// Tag to mark that the work is finished
    static constexpr char workFinishedTag[]=
      "WorkFinished";
    
    /// Waiting for work to be done means to synchronize with the master
    void tellTheMasterWorkIsFinished(const int& threadId) ///< Thread id
    {
      assertPoolOnly(threadId);
      
      minimalLogger(runLog,"finished working (tag: %s)",workFinishedTag);
      
      barrier.sync(workFinishedTag,threadId);
    }
    
    /// Wait that the work assigned to the pool is finished
    void waitForPoolToFinishAssignedWork(const int& threadId) ///< Thread id
    {
      assertMasterOnly(threadId);
      
      if constexpr(DEBUG_THREADS)
	{
	  /// Makes the print sequential across threads
	  THREADS_SCOPE_SEQUENTIAL();
	  
	  minimalLogger(runLog,"waiting for pool to finish the work (tag: %s)",workFinishedTag);
	  mutexUnlock();
	}
      
      // The master signals to the pool that he is waiting for the
      // pool to finish the work
      barrier.sync(workFinishedTag,threadId);
      
      // Mark down that the pool is waiting for work
      isWaitingForWork=
	true;
    }
    
    /// Return whether the pool is waiting for work
    const bool& getIfWaitingForWork() const
    {
      return
	isWaitingForWork;
    }
    
    /// Gives to all threads some work to be done
    ///
    /// The object \c f must be callable, returning void and getting
    /// an integer as a parameter, representing the thread id
    template <typename F>
    void workOn(F f) ///< Function embedding the work
    {
      // Check that the pool is waiting for work
      if(not isWaitingForWork)
	MINIMAL_CRASH("Trying to give work to not-waiting pool!");
      
      // Store the work
      work=
	f;
      
      // Set off the other threads
      tellThePoolWorkIsAssigned(masterThreadId);
      
      work(0);
      
      // Wait that the pool finishes the work
      waitForPoolToFinishAssignedWork(masterThreadId);
    }
    
    /// Split a loop into \c nTrheads chunks, giving each chunk as a work for a corresponding thread
    template <typename Size,           // Type for the range of the loop
	      typename F>              // Type for the work function
    void loopSplit(const Size& beg,  ///< Beginning of the loop
		   const Size& end,  ///< End of the loop
		   F f)              ///< Function to be called, accepting two integers: the first is the thread id, the second the loop argument
    {
      workOn([beg,end,nPieces=this->nActiveThreads(),&f](const int& threadId)
	     {
	       /// Workload for each thread, taking into account the remainder
	       const Size threadLoad=
		 (end-beg+nPieces-1)/nPieces;
	       
	       /// Beginning of the chunk
	       const Size threadBeg=
		 threadLoad*threadId;
	       
	       /// End of the chunk
	       const Size threadEnd=
		 std::min(end,threadBeg+threadLoad);
	       
	       for(Size i=threadBeg;i<threadEnd;i++)
		 f(threadId,i);
	     });
    }
    
    /// Constructor starting the thread pool with a given number of threads
    ThreadPool(int nThreads=std::thread::hardware_concurrency()) :
      pool(1,getThreadTag()),
      nThreads(nThreads),
      barrier(nThreads)
    {
      fill();
    }
    
    /// Destructor emptying the pool
    ~ThreadPool()
    {
      minimalLogger(runLog,"Destroying the pool");
      empty();
    }
  };
  
#else
  
  /// Dummy thread pool
  class ThreadPool
  {
    
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
    ThreadPool(int nThreads=1)
    {
    }
  };
  
#endif
  
  /// Global thread pool
  extern ThreadPool threads;
}

#endif
