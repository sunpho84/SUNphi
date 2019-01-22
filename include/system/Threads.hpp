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

#include <cstdio>
#include <unistd.h>

#include <external/inplace_function.h>
#include <Tuple.hpp>
#include <system/Debug.hpp>
#include <system/Vector.hpp>

namespace SUNphi
{
  class ThreadPool
  {
    /// Wrapper for the pthread barrier functionality
    class Barrier
    {
      pthread_barrier_t barrier;
      
      void rawWait()
      {
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
      
      Barrier(const int& nThreads)
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
      
      void sync()
      {
	rawWait();
      }
      
    };
    
    /// Maximal size of the stack used for thw work
    static constexpr int MAX_POOL_FUNCTION_SIZE=
      128;
    
    /// States if the pool is waiting for work
    bool isWaitingForWork{false};
    
    /// States if the pool is filled
    bool isFilled{false};
    
    /// Type to encapsulate the work to be done
    using Work=
      stdext::inplace_function<void(int),MAX_POOL_FUNCTION_SIZE>;
    
    /// Work to be done
    ///
    /// This incapsulates a function returning void, and getting an
    /// integer as an argument, corresponding to the thread
    Work work;
    
    /// Incapsulate the threads
    ///
    /// At the beginning, the pool contains only the main thread, with
    /// its id. Then when is filled, the pool contains the thread
    /// identifier. This is an opaque number, which cannot serve the
    /// purpose of getting the thread rank in the pool. This is why we define the next function
    Vector<pthread_t> pool{1,pthread_self()};
    
    /// Get the rank of the current thread
    int getThreadRank()
      const
    {
      /// Current pthread
      const pthread_t cur=
	pthread_self();
      
      /// Position in the pool
      int rank=
	pool.findFirst(cur);
      
      // Check that the thread is found
      if(rank==nActiveThreads())
	CRASH("Unable to find cur thread",cur);
      
      return
	rank;
    }
    
    /// Number of threads
    int nThreads;
    
    /// Barrier used by the threads
    Barrier barrier;
    
    using ThreadPars=
      Tuple<ThreadPool*,int>;
    
    static void* swim(void* _ptr)
    {
      ThreadPars* ptr=
	static_cast<ThreadPars*>(_ptr);
      
      ThreadPars& pars=
	*ptr;
      
      ThreadPool& pool=
	*get<0>(pars);
      
      int iThread=
	get<1>(pars);
      
      delete ptr;
      
      printf("Thread: %d %d, %lu %p\n",iThread,pool.getThreadRank(),pthread_self(),_ptr);
      
      pool.waitForWorktoDo();
      
      // tobefixed
      // while(pool.isWaitingForWork)
      // 	{
      // 	  pool.waitForWorktoDo();
	  
      // 	  pool.work(iThread);
      // 	};
      
      return
	nullptr;
    }
    
    void fill(const pthread_attr_t* attr=nullptr)
    {
      if(isFilled)
	CRASH("Cannot fill again the pool!");
      
      isFilled=
	true;
      
      isWaitingForWork=
	true;
      
      pool.resize(nThreads);
      
      for(int iThread=1;iThread<nThreads;iThread++)
	{
	  std::cout<<iThread<<" spawned"<<std::endl;
	  // sleep(1);
	  
	  ThreadPars* pack=
	    new ThreadPars{this,iThread};
	  
	  if(pthread_create(&pool[iThread],attr,swim,pack)!=0)
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
    
    void empty()
    {
      barrier.sync();
      
      isWaitingForWork=
	false;
      
      if(not isFilled)
	CRASH("Cannot empty an empty pool!");
      
      isFilled=
	false;
      
      for(int iThread=1;iThread<nThreads;iThread++)
	{
	  if(pthread_join(pool[iThread],nullptr)!=0)
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
	  
	  std::cout<<iThread<<" destroyed"<<std::endl;
	}
      
      pool.resize(1);
    }
    
  public:
    
    int nActiveThreads()
      const
    {
      return
	pool.size();
    }
    
    void waitForWorktoDo()
    {
      barrier.sync();
    }
    
    void raisePoolAttention()
    {
      barrier.sync();
    }
    
    template <typename F>
    void workOn(F f)
    {
      if(not isWaitingForWork)
	CRASH("Trying to give work to not-waiting pool!");
      
      work=
	f;
      
      isWaitingForWork=
	false;
      
      raisePoolAttention();
      
      //f(iThread);
    }
    
    template <typename Size,
	      typename F>
    void loopSplit(const Size& beg,
		   const Size& end,
		   F f)
    {
      workOn([beg,end,nPieces=this->nActiveThreads(),&f](int iThread)
	     {
	       const Size threadLoad=
		 (end-beg+nPieces-1)/nPieces;
	       
	       const Size threadBeg=
		 threadLoad*iThread;
	       
	       const Size threadEnd=
		 std::min(end,threadBeg+threadLoad);
	       
	       for(Size i=threadBeg;i<threadEnd;i++)
		 f(i);
	     });
    }
    
    ThreadPool(int nThreads=std::thread::hardware_concurrency()) :
      nThreads(nThreads),barrier(nThreads)
    {
      fill();
    }
    
    ~ThreadPool()
    {
      std::cout<<"Want to destroy"<<std::endl;
      empty();
    }
  };
}

#endif
