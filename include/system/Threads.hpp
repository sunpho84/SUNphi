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
#include <Tuple.hpp>
#include <system/Debug.hpp>
#include <system/Vector.hpp>

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
      
      /// Synchronize, possibly checking the name of the barrier
      void sync()
      {
	rawSync();
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
    
    /// Pair of parameters containing the threadpool and the thread id
    using ThreadPars=
      Tuple<ThreadPool*,int>;
    
    /// Function called when starting a thread
    ///
    /// When called, get the thread pool and the thread id as
    /// arguments through the function parameter. This is expcted to
    /// be allocated outside through a \c new call, so it is deleted
    /// after taking reference to the pool, and checking the thread id.
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
      
      /// Copy the thread id
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
      
      for(int iThread=1;iThread<nThreads;iThread++)
	{
	  std::cout<<iThread<<" spawned"<<std::endl;
	  
	  // Allocates the parameters of the thread
	  ThreadPars* pars=
	    new ThreadPars{this,iThread};
	  
	  if(pthread_create(&pool[iThread],attr,swim,pars)!=0)
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
    
    /// Empty the thread pool
    void empty()
    {
      // Check that the pool is not empty
      if(not isFilled)
	CRASH("Cannot empty an empty pool!");
      
      // Mark that the pool is not filled
      isFilled=
	false;
      
      // Temporary name to force the other threads go out of the pool
      barrier.sync();
      
      // Mark that the pool is not waiting any more for work
      isWaitingForWork=
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
      
      // Resize down the pool
      pool.resize(1);
    }
    
  public:
    
    /// Gets the number of allocated threads
    int nActiveThreads()
      const
    {
      return
	pool.size();
    }
    
    /// Waiting for work to be done means to synchronize with the master
    void waitForWorktoDo()
    {
      barrier.sync();
    }
    
    /// The master signals to the pool to start work by synchronizing with it
    void raisePoolAttention()
    {
      barrier.sync();
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
      
      // Mark that the pool is not waiting for job to be done
      isWaitingForWork=
	false;
      
      // Set off the other threads
      raisePoolAttention();
      
      //f(iThread);
    }
    
    /// Split a loop into \c nTrheads chunks, giving each chunk as a work for a corresponding thread
    template <typename Size,           // Type for the range of the loop
	      typename F>              // Type for the work function
    void loopSplit(const Size& beg,  ///< Beginning of the loop
		   const Size& end,  ///< End of the loop
		   F f)              ///< Function to be called
    {
      workOn([beg,end,nPieces=this->nActiveThreads(),&f](int iThread)
	     {
	       /// Workload for each thread, taking into account the remainder
	       const Size threadLoad=
		 (end-beg+nPieces-1)/nPieces;
	       
	       /// Beginning of the chunk
	       const Size threadBeg=
		 threadLoad*iThread;
	       
	       /// End of the chunk
	       const Size threadEnd=
		 std::min(end,threadBeg+threadLoad);
	       
	       for(Size i=threadBeg;i<threadEnd;i++)
		 f(i);
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
      std::cout<<"Want to destroy"<<std::endl;
      empty();
    }
  };
}

#endif
