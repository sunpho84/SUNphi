#ifndef _THREADS_HPP
#define _THREADS_HPP

/// \file Threads.hpp
///
/// \brief Provides a thread pool
///

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#ifdef USE_THREADS
 #include <pthread.h>
 #include <thread>
#endif

#include <external/inplace_function.h>

#include <containers/Vector.hpp>
#include <system/Debug.hpp>
#include <Tuple.hpp>

namespace SUNphi
{
  /// Contains a thread pool
  class ThreadPool
  {
    /// Wrapper for the pthread barrier functionality
    class Barrier
    {
      /// Raw pthread barrier
      pthread_barrier_t barrier;
      
#ifdef DEBUG_MODE
      
      /// Value used to check the barrier
      const char* currBarrName;
      
#endif
      
      /// Raw synchronization, simply wait that all threads call this
      void rawSync()
      {
	/// Call the barrier and get the result
	const int rc=
	  pthread_barrier_wait(&barrier);
	
	if(rc!=0 and rc!=PTHREAD_BARRIER_SERIAL_THREAD)
	  switch(errno)
	    {
	    case EINVAL:
	      CRASH("The value specified by barrier does not refer to an initialized barrier object");
	      break;
	    default:
	      CRASH("Other error");
	    }
      }
      
    public:
      
      ///Build the barrier for \c nThreads threads
      Barrier(const int& nThreads) ///< Number of threads for which the barrier is defined
      {
	if(pthread_barrier_init(&barrier,nullptr,nThreads)!=0)
	  switch(errno)
	    {
	    case EBUSY:
	      CRASH("The implementation has detected an attempt to reinitialize a barrier while it is in use");
	      break;
	    case EAGAIN:
	      CRASH("The system lacks the necessary resources to initialize another barrier");
	      break;
	    case EINVAL:
	      CRASH("The value specified by count is equal to zero, or the value specified by attr is invalid");
	      break;
	    case ENOMEM:
	      CRASH("Insufficient memory exists to initialize the barrier");
	      break;
	    default:
	      CRASH("Other error");
	    }
      }
      
      /// Destroys the barrier
      ~Barrier()
      {
	if(pthread_barrier_destroy(&barrier)!=0)
	  switch(errno)
	    {
	    case EBUSY:
	      CRASH("The implementation has detected an attempt to destroy a barrier while it is in use");
	      break;
	    case EINVAL:
	      CRASH("The value specified by barrier is invalid");
	      break;
	    default:
	      CRASH("Other error");
	    }
      }
      
      /// Synchronize, without checking the name of the barrier
      void sync()
      {
	rawSync();
      }
      
      /// Synchronize checking the name of the barrier
      void sync(const char* barrName, //< Name of the barrier
		const int& threadId) ///< Id of the thread used coming to check
      {
	rawSync();
	
#ifdef DEBUG_MODE
	
	if(threadId==masterThreadId)
	  currBarrName=
	    barrName;
	
	rawSync();
	
	if(currBarrName!=barrName)
	  CRASH("Thread id",threadId,"was expecting",currBarrName,"but",barrName,"encountered");
	
#endif
	
      }
    };
    
    /// Maximal size of the stack used for thw work
    static constexpr int MAX_POOL_FUNCTION_SIZE=
      128;
    
    /// States if the pool is waiting for work
    bool isWaitingForWork{false};
    
    /// States if the pool is filled
    bool isFilled{false};
    
    /// Thread id of master thread
    static constexpr int masterThreadId=
      0;
    
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
    Vector<pthread_t> pool{1,pthread_self()};
    
    /// Return the thread tag
    static pthread_t getThreadTag()
    {
      return
	pthread_self();
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
	CRASH("Unable to find thread with tag",threadTag);
      
      return
	threadId;
    }
    
    /// Number of threads
    int nThreads;
    
    /// Barrier used by the threads
    Barrier barrier;
    
    /// Check that only the pool is accessing to a checkpoint
    void checkPoolOnly(const int& threadId) ///< Calling thread
      const
    {
      if(threadId==masterThreadId)
	CRASH("Only pool threads are allowed");
    }
    
    /// Check that only the master thread is accessing to a checkpoint
    void checkMasterOnly(const int& threadId) ///< Calling thread
      const
    {
      if(threadId!=masterThreadId)
	CRASH("Only master thread is allowed, but thread",threadId,"is trying to act");
    }
    
    /// Pair of parameters containing the threadpool and the thread id
    using ThreadPars=
      Tuple<ThreadPool*,int>;
    
    /// Function called when starting a thread
    ///
    /// When called, get the thread pool and the thread thread as
    /// arguments through the function parameter. This is expcted to
    /// be allocated outside through a \c new call, so it is deleted
    /// after taking reference to the pool, and checking the thread thread.
    ///
    /// All threads but the master one swim in this pool back and forth,
    /// waiting for job to be done.
    static void* swim(void* _ptr) ///< Initialization data
    {
      /// Cast the \c void pointer to the tuple
      ThreadPars* ptr=
	static_cast<ThreadPars*>(_ptr);
      
      /// Takes a reference to the parameters
      ThreadPars& pars=
	*ptr;
      
      /// Takes a reference to the pool
      ThreadPool& pool=
	*get<0>(pars);
      
      /// Copy the thread thread
      int threadId=
	get<1>(pars);
      
      delete ptr;
      
      printf("Thread id: %d (check: %d) entering the pool, %lu %p\n",threadId,pool.getThreadId(),pthread_self(),_ptr);
      
      /// Work until asked to empty
      bool keepSwimming=
	pool.isFilled;
      
      while(keepSwimming)
	{
	  pool.waitForWorkToBeAssigned(threadId);
	  
	  keepSwimming=
	    pool.isFilled;
	  
	  printf("Thread id: %d keep swimming: %d\n",threadId,keepSwimming);
	  
	  if(keepSwimming)
	    {
	      pool.work(threadId);
	      
	      pool.tellTheMasterWorkIsFinished(threadId);
	    }
	}
      
      printf("Thread: %d (check: %d) exiting the pool, %lu %p\n",threadId,pool.getThreadId(),pthread_self(),_ptr);
      
      return
	nullptr;
    }
    
    /// Fill the pool with the number of thread assigned
    void fill(const pthread_attr_t* attr=nullptr) ///< Possible attributes of the threads
    {
      // Checks that the pool is not filled
      if(isFilled)
	CRASH("Cannot fill again the pool!");
      
      // Marks the pool as filled
      isFilled=
	true;
      
      // Marks the pool is waiting for job to be done
      isWaitingForWork=
	true;
      
      // Resize the pool to contain all threads
      pool.resize(nThreads);
      
      for(int threadId=1;threadId<nThreads;threadId++)
	{
	  printf("thread of id %d spwawned\n",threadId);
	  
	  // Allocates the parameters of the thread
	  ThreadPars* pars=
	    new ThreadPars{this,threadId};
	  
	  if(pthread_create(&pool[threadId],attr,swim,pars)!=0)
	    switch(errno)
	      {
	      case EAGAIN:
		CRASH("A system-imposed limit on the number of threads was encountered");
		break;
	      case EINVAL:
		CRASH("Invalid settings in attr");
		break;
	      case EPERM:
		CRASH("No permission to set the scheduling policy and parameters specified in attr");
		break;
	      default:
		CRASH("Other error");
	      }
	}
    }
    
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
	CRASH("Cannot empty an empty pool!");
      
      // Mark that the pool is not filled
      isFilled=
	false;
      
      /// Stop the pool from working
      tellThePoolNotToWorkAnyLonger(masterThreadId);
      
      for(int threadId=1;threadId<nThreads;threadId++)
	{
	  if(pthread_join(pool[threadId],nullptr)!=0)
	    switch(errno)
	      {
	      case EDEADLK:
		CRASH("A deadlock was detected");
		break;
	      case EINVAL:
		CRASH("Thread is not a joinable thread, or another thread is already waiting to join with this thread");
		break;
	      case ESRCH:
		CRASH("No thread with the ID thread could be found");
		break;
	      default:
		CRASH("Other error");
	      }
	  
	  printf("Thread of id %d destroyed\n",threadId);
	}
      
      // Resize down the pool
      pool.resize(1);
    }
    
  public:
    
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
    
    /// Tag to mark that assignment has been done
    static constexpr char workAssignmentTag[]=
      "WorkAssOrNoMoreWork";
    
    /// Tag to mark that no more work has to be done
    static constexpr auto& workNoMoreTag=
      workAssignmentTag;
    
    /// Start the work for the other threads
    void tellThePoolWorkIsAssigned(const int& threadId) ///< Thread id
    {
      checkMasterOnly(threadId);
      
      printf("Thread of id %d is telling the pool that work has been assigned (tag: %s)\n",threadId,workAssignmentTag);
      
      // Mark down that the pool is not waiting for work
      isWaitingForWork=
	false;
      
      // The master signals to the pool to start work by synchronizing with it
      barrier.sync(workAssignmentTag,threadId);
    }
    
    /// Waiting for work to be done means to synchronize with the master
    void waitForWorkToBeAssigned(const int& threadId) ///< Thread id
    {
      checkPoolOnly(threadId);
      
      printf("Thread of id %d is waiting the pool for work to be assigned (tag %s)\n",threadId,workAssignmentTag);
      
      barrier.sync(workAssignmentTag,threadId);
    }
    
    /// Stop the pool from working
    void tellThePoolNotToWorkAnyLonger(const int& threadId) ///< Thread id
    {
      checkMasterOnly(threadId);
      
      if(not isWaitingForWork)
	CRASH("We cannot stop a working pool");
      
      printf("Thread of id %d is telling the pool not to work any longer (tag: %s)\n",threadId,workNoMoreTag);
      
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
      checkPoolOnly(threadId);
      
      printf("Thread of id %d has finished working (tag: %s)\n",threadId,workFinishedTag);
      
      barrier.sync(workFinishedTag,threadId);
    }
    
    /// Wait that the work assigned to the pool is finished
    void waitForPoolToFinishAssignedWork(const int& threadId) ///< Thread id
    {
      checkMasterOnly(threadId);
      
      printf("Thread of id %d is waiting for work to be finished (tag: %s)\n",threadId,workFinishedTag);
      
      // The master signals to the pool that he is waiting for the
      // pool to finish the work
      barrier.sync(workFinishedTag,threadId);
      
      // Mark down that the pool is waiting for work
      isWaitingForWork=
	true;
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
	CRASH("Trying to give work to not-waiting pool!");
      
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
      nThreads(nThreads),barrier(nThreads)
    {
      fill();
    }
    
    /// Destructor emptying the pool
    ~ThreadPool()
    {
      printf("Want to destroy\n");
      empty();
    }
  };
}

#endif
