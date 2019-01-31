#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#ifdef USE_THREADS
 #include <pthread.h>
 #include <thread>
#endif

#include <external/inplace_function.h>

#include <containers/Vector.hpp>
#include <ios/Logger.hpp>
#include <system/Debug.hpp>
#include <Tuple.hpp>

namespace SUNphi
{
  static void* swim(void* _ptr)
    {
      /// Cast the \c void pointer to the tuple
      Threads::ThreadPars* ptr=
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
      
      logger<<"Thread id: "<<threadId<<" (check: "<<pool.getThreadId()<<") entering the pool, "<<pthread_self()<<"\n";
      
      /// Work until asked to empty
      bool keepSwimming=
	pool.isFilled;
      
      while(keepSwimming)
	{
	  pool.waitForWorkToBeAssigned(threadId);
	  
	  keepSwimming=
	    pool.isFilled;
	  
	  logger<<"Thread id: "<<threadId<<" keep swimming: "<<keepSwimming<<"\n";
	  
	  if(keepSwimming)
	    {
	      pool.work(threadId);
	      
	      pool.tellTheMasterWorkIsFinished(threadId);
	    }
	}
      
      logger<<"Thread: "<<threadId<<" (check: "<<pool.getThreadId()<<") exiting the pool, "<<pthread_self()<<"\n";
      
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
	  logger<<"thread of id "<<threadId<<" spwawned\n";
	  
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
	  
	  logger<<"Thread of id "<<threadId<<" destroyed\n";
	}
      
      // Resize down the pool
      pool.resize(1);
    }
    
    /// Global mutex
    Mutex mutex;
    
  public:
    
    /// Lock the mutex
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
      
      logger<<"Thread of id "<<threadId<<" is telling the pool that work has been assigned (tag: "<<workAssignmentTag<<"\n";
      
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
      
      logger<<"Thread of id "<<threadId<<" is waiting the pool for work to be assigned (tag "<<workAssignmentTag<<"\n";
      
      barrier.sync(workAssignmentTag,threadId);
    }
    
    /// Stop the pool from working
    void tellThePoolNotToWorkAnyLonger(const int& threadId) ///< Thread id
    {
      checkMasterOnly(threadId);
      
      if(not isWaitingForWork)
	CRASH("We cannot stop a working pool");
      
      logger<<"Thread of id "<<threadId<<" is telling the pool not to work any longer (tag: "<<workNoMoreTag<<"\n";
      
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
      
      logger<<"Thread of id "<<threadId<<" has finished working (tag: "<<workFinishedTag<<")\n";
      
      barrier.sync(workFinishedTag,threadId);
    }
    
    /// Wait that the work assigned to the pool is finished
    void waitForPoolToFinishAssignedWork(const int& threadId) ///< Thread id
    {
      checkMasterOnly(threadId);
      
      if constexpr(DEBUG_THREADS)
	{
	  /// Makes the print sequential across threads
	  THREADS_SCOPE_SEQUENTIAL();
	  
	  logger<<"Thread of id "<<threadId<<" is waiting for work to be finished (tag: "<<workFinishedTag<<")\n";
	  mutexUnlock();
	}
      
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
      logger<<"Destroying the pool\n";
      empty();
    }
  };
  
  /// Global threads
  extern ThreadPool threads;
}

#endif
