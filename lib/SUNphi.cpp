#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file SUNphi.cpp
///
/// \brief Implements the parts of code which require dedicated compilation units

#include <cstdarg>
#include <cstdio>

#include <gitInfo.hpp>

#include <Threads.hpp>
#include <debug/Crash.hpp>
#include <ios/Logger.hpp>
#include <ios/MinimalLogger.hpp>
#include <ios/TextColors.hpp>
#include <random/TrueRandomGenerator.hpp>
#include <system/Mpi.hpp>
#include <system/Timer.hpp>
#include <utility/Aliver.hpp>
#include <utility/SingleInstance.hpp>

#ifndef CONFIG_TIME
 /// Null time
 #define CONFIG_TIME				\
 ""
#endif

#ifndef CONFIG_FLAGS
 /// Null flags
 #define CONFIG_FLAGS				\
  ""
#endif

namespace SUNphi
{
  void minimalLogger(Logger& logger,
		     const char* format,
		     ...)
  {
    /// Starts the variadic arguments
    va_list ap;
    va_start(ap,format);
    
    logger.printVariadicMessage(format,ap);
    
    va_end(ap);
  }
  
  void minimalCrash(const char* path,
		    const int line,
		    const char* funcName,
		    const char* format,
		    ...)
  {
    /// Starts the variadic arguments
    va_list ap;
    va_start(ap,format);
    
    (runLog()<<Crasher(path,line,funcName)).printVariadicMessage(format,ap);
    
    va_end(ap);
  }
  
#ifdef USE_THREADS
  void ThreadPool::fill(const pthread_attr_t* attr)
  {
    {
      ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(runLog);
      
      runLog()<<"Filling the thread pool with "<<nThreads<<" threads";
      
      // Checks that the pool is not filled
      if(isFilled)
	MINIMAL_CRASH("Cannot fill again the pool!");
      
      // Resize the pool to contain all threads
      pool.resize(nThreads,0);
      
      // Marks the pool as filled, even if we are still filling it, this will keep the threads swimming
      isFilled=
	true;
      
      for(int threadId=1;threadId<nThreads;threadId++)
	{
	  //runLog()<<"thread of id "<<threadId<<" spwawned\n";
	  
	  // Allocates the parameters of the thread
	  ThreadPars* pars=
	    new ThreadPars{this,threadId};
	  
	  if(pthread_create(&pool[threadId],attr,threadPoolSwim,pars)!=0)
	    MINIMAL_CRASH_STDLIBERR("creating the thread");
	}
      
      waitPoolToBeFilled(masterThreadId);
    }
    
    // Marks the pool is waiting for job to be done
    isWaitingForWork=
      true;
  }
#endif // USE_THREADS
  
  int aliverHelper()
  {
    return
      0;
  }
  
  /// Class used to provocate initialization of Mpi
  class Aliver : public SingleInstance<Aliver>
  {
    
    /// Prints the banner
    void printBanner()
      const
    {
      runLog()<<"";
      runLog()<<TextColor::BROWN<<"          ▄▄        ▄█▄        ▄▄        \t"<<TextColor::PURPLE<< "                 ▄█▄                  ";
      runLog()<<TextColor::BROWN<<"          █░█       █░█       █░█         \t"<<TextColor::PURPLE<< "                 █░█                   ";
      runLog()<<TextColor::BROWN<<"     ▄▄    █░█      █░█      █░█    ▄▄   \t"<<TextColor::PURPLE<<  "                 █░█                  ";
      runLog()<<TextColor::BROWN<<"     █░█    █░█     █░█     █░█    █░█   \t"<<TextColor::PURPLE<<  "                 █░█                  ";
      runLog()<<TextColor::BROWN<<"      █░█    █░█  ███████  █░█    █░█    \t"<<TextColor::PURPLE<<  "               ███████                ";
      runLog()<<TextColor::BROWN<<"       █░█    █████░░░░░█████    █░█     \t"<<TextColor::PURPLE<<  "           █████░█░█░█████            ";
      runLog()<<TextColor::BROWN<<"        █░█  ██░░░░░░░░░░░░░██  █░█      \t"<<TextColor::PURPLE<<  "          ██░░░░░█░█░░░░░██           ";
      runLog()<<TextColor::BROWN<<"         █░██░░░░░░░░░░░░░░░░░██░█       \t"<<TextColor::PURPLE<<  "        ██░░░░░░░█░█░░░░░░░██         ";
      runLog()<<TextColor::BROWN<<"    ▄▄▄▄▄▄███████████░███████████▄▄▄▄▄▄ \t" <<TextColor::PURPLE<< "       ██░░░░░░░░█░█░░░░░░░░██        ";
      runLog()<<TextColor::BROWN<<"   █░░░░░░█░████████░░░████████░█░░░░░░█ \t"<<TextColor::PURPLE<<  "       █░░░░░░░░░█░█░░░░░░░░░█        ";
      runLog()<<TextColor::BROWN<<"    ▀▀▀▀▀▀█░░░████░░░░░░░████░░░█▀▀▀▀▀▀ \t" <<TextColor::PURPLE<< "       █░░░░░░░░░█░█░░░░░░░░░█        ";
      runLog()<<TextColor::BROWN<<"          ██░░░░░░░░░░░░░░░░░░░░█        \t"<<TextColor::PURPLE<<  "       ██░░░░░░░░█░█░░░░░░░░░█        ";
      runLog()<<TextColor::BROWN<<"         █░██░░░░░███████░░░░░░█░█       \t"<<TextColor::PURPLE<<  "        ██░░░░░░░█░█░░░░░░░░█         ";
      runLog()<<TextColor::BROWN<<"        █░█  █░░░░░░░░░░░░░░░██ █░█      \t"<<TextColor::PURPLE<<  "          █░░░░░░█░█░░░░░░██          ";
      runLog()<<TextColor::BROWN<<"       █░█    ██░░░░░░░░░░░██    █░█     \t"<<TextColor::PURPLE<<  "           ██░░░░█░█░░░░██            ";
      runLog()<<TextColor::BROWN<<"      █░█     █░███████████░█     █░█    \t"<<TextColor::PURPLE<<  "             ███████████              ";
      runLog()<<TextColor::BROWN<<"     █░█     █░█    █░█    █░█     █░█   \t"<<TextColor::PURPLE<<  "                 █░█                  ";
      runLog()<<TextColor::BROWN<<"     ▀▀     █░█     █░█     █░█     ▀▀  \t" <<TextColor::PURPLE<<  "                 █░█                  ";
      runLog()<<TextColor::BROWN<<"           █░█      █░█      █░█        \t" <<TextColor::PURPLE<<  "                 █░█                 ";
      runLog()<<TextColor::BROWN<<"          █░█       █░█       █░█       \t" <<TextColor::PURPLE<<  "                 █░█                 ";
      runLog()<<TextColor::BROWN<<"          ▀▀        ▀█▀        ▀▀       \t" <<TextColor::PURPLE<< "                 ▀█▀                ";
      runLog()<< "";
    }
    
    /// Prints the version, and contacts
    void printVersionContacts()
      const
    {
      runLog()<<"\nInitializing "<<PACKAGE_NAME<<" library, v"<<PACKAGE_VERSION<<", send bug report to <"<<PACKAGE_BUGREPORT<<">";
    }
    
    /// Prints the git info
    void printGitInfo()
      const
    {
      runLog()<<"Commit "<<GIT_HASH<<" made at "<<GIT_TIME<<" by "<<GIT_COMMITTER<<" with message: \""<<GIT_LOG<<"\"";
    }
    
    /// Prints configure info
    void printConfigurePars()
      const
    {
      runLog()<<"Configured at "<<CONFIG_TIME<<" with flags: "<<CONFIG_FLAGS<<"";
    }
    
    /// Says bye bye
    void printBailout()
      const
    {
      runLog()<<"\n Ciao!\n";
    }
    
  public:
    
    /// Creates
    Aliver()
    {
      printBanner();
      printVersionContacts();
      printGitInfo();
      printConfigurePars();
      
      threads.workOn([](const int threadID){runLog()<<"ANNA";});
      
      {
	ALLOWS_ALL_THREADS_TO_PRINT_FOR_THIS_SCOPE(runLog);
	ALLOWS_ALL_RANKS_TO_PRINT_FOR_THIS_SCOPE(runLog);
	
	threads.workOn([](const int threadID){runLog()<<"ANNA";});
      }
      // threads.loopSplit(0,10,[](const int& rank,const int& i){printf("Rank %d prints again %d\n",rank,i);});
    }
    
    /// Destroyer
    ~Aliver()
    {
      printBailout();
    }
  };
  
  /// Global timings
  Timer timings("Total time",Timer::NO_FATHER,Timer::UNSTOPPABLE);
  
  Logger Logger::fakeLogger("/dev/null");
  
  /// Global logger
  Logger runLog("/dev/stdout");
  
  /// Global MPI
  Mpi mpi;
  
  /// Global thrads
  ThreadPool threads;
  
  /// Global true random generator
  TrueRandomGenerator trueRandomGenerator;
  
  /// Presentation of the library
  Aliver aliver;
}

