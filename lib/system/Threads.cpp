#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <ios/Logger.hpp>
#include <threads/ThreadPool.hpp>

namespace SUNphi
{
  void* threadPoolSwim(void* _ptr)
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
	
	runLog<<" keep swimming: "<<keepSwimming<<"\n";
	
	if(keepSwimming)
	  {
	    pool.work(threadId);
	    
	    pool.tellTheMasterWorkIsFinished(threadId);
	  }
      }
    
    //runLog<<"exiting the pool";
    
    return
      nullptr;
  }
  
  void ThreadPool::fill(const pthread_attr_t* attr)
    {
      {
	ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(runLog);
	
	runLog<<"Filling the thread pool with "<<nThreads<<" threads";
	
	// Checks that the pool is not filled
	if(isFilled)
	  CRASH("Cannot fill again the pool!");
	
	// Resize the pool to contain all threads
	pool.resize(nThreads,0);
	
	// Marks the pool as filled, even if we are still filling it, this will keep the threads swimming
	isFilled=
	  true;
	
	for(int threadId=1;threadId<nThreads;threadId++)
	  {
	    //runLog<<"thread of id "<<threadId<<" spwawned\n";
	    
	    // Allocates the parameters of the thread
	    ThreadPars* pars=
	      new ThreadPars{this,threadId};
	    
	    if(pthread_create(&pool[threadId],attr,threadPoolSwim,pars)!=0)
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
	
	waitPoolToBeFilled(masterThreadId);
      }
      
      // Marks the pool is waiting for job to be done
      isWaitingForWork=
	true;
      
    }
}
