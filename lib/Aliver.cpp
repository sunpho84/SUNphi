#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file Aliver.cpp
///
/// \brief Starts the library and close it

#include <gitInfo.hpp>

#include <ios/Logger.hpp>
#include <system/MpiImpl.hpp>
#include <system/Threads.hpp>
#include <system/Timer.hpp>

#ifndef CONFIG_TIME
 /// Null time
 #define CONFIG_TIME \
 ""
#endif

#ifndef CONFIG_FLAGS
 /// Null flags
 #define CONFIG_FLAGS				\
  ""
#endif

/// Namespace of the SUNphi library
namespace SUNphi
{
  /// Statistics
  Timer timings("Total time",Timer::NO_FATHER,Timer::UNSTOPPABLE);
  
  /// Global logger
  ///
  /// \todo check if we can refer to stdout
  Logger logger("/dev/stdout");
  
  int aliverHelper()
  {
    return
      0;
  }
  
  /// Prints the banner
  void printBanner()
  {
    if(Mpi::isMasterRank())
      printf(
	     "       ▄▄        ▄█▄        ▄▄        \t" "                 ▄█▄                  \n"
	     "       █░█       █░█       █░█         \t" "                 █░█                   \n"
	     "  ▄▄    █░█      █░█      █░█    ▄▄   \t"  "                 █░█                  \n"
	     "  █░█    █░█     █░█     █░█    █░█   \t"  "                 █░█                  \n"
	     "   █░█    █░█  ███████  █░█    █░█    \t"  "               ███████                \n"
	     "    █░█    █████░░░░░█████    █░█     \t"  "           █████░█░█░█████            \n"
	     "     █░█  ██░░░░░░░░░░░░░██  █░█      \t"  "          ██░░░░░█░█░░░░░██           \n"
	     "      █░██░░░░░░░░░░░░░░░░░██░█       \t"  "        ██░░░░░░░█░█░░░░░░░██         \n"
	     " ▄▄▄▄▄▄███████████░███████████▄▄▄▄▄▄ \t"  "       ██░░░░░░░░█░█░░░░░░░░██        \n"
	     "█░░░░░░█░████████░░░████████░█░░░░░░█ \t"  "       █░░░░░░░░░█░█░░░░░░░░░█        \n"
	     " ▀▀▀▀▀▀█░░░████░░░░░░░████░░░█▀▀▀▀▀▀ \t"  "       █░░░░░░░░░█░█░░░░░░░░░█        \n"
	     "       ██░░░░░░░░░░░░░░░░░░░░█        \t"  "       ██░░░░░░░░█░█░░░░░░░░░█        \n"
	     "      █░██░░░░░███████░░░░░░█░█       \t"  "        ██░░░░░░░█░█░░░░░░░░█         \n"
	     "     █░█  █░░░░░░░░░░░░░░░██ █░█      \t"  "          █░░░░░░█░█░░░░░░██          \n"
	     "    █░█    ██░░░░░░░░░░░██    █░█     \t"  "           ██░░░░█░█░░░░██            \n"
	     "   █░█     █░███████████░█     █░█    \t"  "             ███████████              \n"
	     "  █░█     █░█    █░█    █░█     █░█   \t"  "                 █░█                  \n"
	     "  ▀▀     █░█     █░█     █░█     ▀▀  \t"   "                 █░█                  \n"
	     "        █░█      █░█      █░█        \t"   "                 █░█                 \n"
	     "       █░█       █░█       █░█       \t"   "                 █░█                 \n"
	     "       ▀▀        ▀█▀        ▀▀      \t"    "                 ▀█▀                \n");
  }
  
  /// Prints the version, and contacts
  void printVersionContacts()
  {
    if(Mpi::isMasterRank())
      printf("\nInitializing %s library, v" PACKAGE_VERSION ", send bug report to <" PACKAGE_BUGREPORT ">\n",PACKAGE_NAME);
  }
  
  /// Prints the git info
  void printGitInfo()
  {
    if(Mpi::isMasterRank())
      printf("Commit %s made at %s by %s with message: \"%s\"\n",GIT_HASH,GIT_TIME,GIT_COMMITTER,GIT_LOG);
  }
  
  /// Prints configure info
  void printConfigurePars()
  {
    if(Mpi::isMasterRank())
      printf("Configured at %s with flags: %s\n",CONFIG_TIME,CONFIG_FLAGS);
  }
  
  /// Initialize the SUNphi library
  __attribute__((constructor))
  void makeAlive()
  {
    Mpi::init();
    
    printBanner();
    printVersionContacts();
    printGitInfo();
    printConfigurePars();
    
    ThreadPool threads;
    
    threads.loopSplit(0,10,[](const int& rank,const int& i){printf("Rank %d prints %d\n",rank,i);});
    
    threads.loopSplit(0,10,[](const int& rank,const int& i){printf("Rank %d prints again %d\n",rank,i);});
    
  }
  
  /// Says bye bye
  void printBailout()
  {
    if(Mpi::isMasterRank())
      printf("\n Ciao!\n\n");
  }
  
  /// Finalize the SUNphi library
  __attribute__((destructor))
  void makeDead()
  {
    Mpi::finalize();
    
    printBailout();
  }
}

