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
	  switch(rc)
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
	const int rc=
	  pthread_barrier_init(&barrier,nullptr,nThreads);
	
	if(rc!=0)
	  switch(rc)
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
	const int rc=
	  pthread_barrier_destroy(&barrier);
	
	if(rc!=0)
	  switch(rc)
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
      
      void wait()
      {
	rawWait();
      }
      
    };
    
    /// Maximal size of the stack used for thw work
    static constexpr int MAX_POOL_FUNCTION_SIZE=
      128;
    
    bool poolIsFilled{false};
    
    /// Type to encapsulate the work to be done
    using Work=
      stdext::inplace_function<void(int),MAX_POOL_FUNCTION_SIZE>;
    
    /// Work to be done
    ///
    /// This incapsulates a function returning void, and getting an
    /// integer as an argument, corresponding to the thread
    Work work;
    
    /// Incapsulate the threads
    Vector<pthread_t> threads;
    
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
      
      pool.barrier.wait();
      
      printf("Thread: %d, %lu %p\n",iThread,pthread_self(),_ptr);
      
      return
	nullptr;
    }
    
    void fill(const pthread_attr_t* attr=nullptr)
    {
      if(poolIsFilled)
	CRASH("Cannot fill again the pool!");
      
      poolIsFilled=
	true;
      
      for(int iThread=1;iThread<nThreads();iThread++)
	{
	  std::cout<<"iThread spawned"<<std::endl;
	  // sleep(1);
	  
	  ThreadPars* pack=
	    new ThreadPars{this,iThread};
	  
	  const int rc=
	    pthread_create(&threads[iThread],attr,swim,pack);
	  
	  if(rc!=0)
	    switch(rc)
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
      barrier.wait();
      
      if(not poolIsFilled)
	CRASH("Cannot empty an empty pool!");
      
      poolIsFilled=
	false;
      
      for(int iThread=1;iThread<nThreads();iThread++)
	{
	  const int rc=
	    pthread_join(threads[iThread],nullptr);
	  
	  if(rc!=0)
	    switch(rc)
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
	}
    }
    
  public:
    
    decltype(threads)::Size nThreads()
      const
    {
      return
	threads.size();
    }
    
    template <typename Size,
	      typename F>
    void loopDo(const Size& beg,const Size& end,F f)
    {
      work=
	[beg,end,nThreads=this->nThreads(),&f](int iThread)
	{
	  const Size threadLoad=
	  (end-beg+nThreads-1)/nThreads;
	  
	  const Size threadBeg=
	  threadLoad*iThread;
	  
	  const Size threadEnd=
	  std::min(end,threadBeg+threadLoad);
	  
	  for(Size i=threadBeg;i<threadEnd;i++)
	    f(i);
	};
      
    }
    
    ThreadPool(int nThreads=std::thread::hardware_concurrency()) :
      threads(nThreads),barrier(nThreads)
    {
      threads.resize(nThreads);
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
