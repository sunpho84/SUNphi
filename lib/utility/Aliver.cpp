#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file Aliver.cpp
///
/// \brief Starts the library and close it

#include <gitInfo.hpp>

#include <ios/Logger.hpp>
#include <system/Mpi.hpp>
#include <system/Threads.hpp>
#include <system/Timer.hpp>
#include <utility/Aliver.hpp>
#include <utility/SingleInstance.hpp>

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
      logger<<"";
      logger<<"          ▄▄        ▄█▄        ▄▄        \t" "                 ▄█▄                  ";
      logger<<"          █░█       █░█       █░█         \t" "                 █░█                   ";
      logger<<"     ▄▄    █░█      █░█      █░█    ▄▄   \t"  "                 █░█                  ";
      logger<<"     █░█    █░█     █░█     █░█    █░█   \t"  "                 █░█                  ";
      logger<<"      █░█    █░█  ███████  █░█    █░█    \t"  "               ███████                ";
      logger<<"       █░█    █████░░░░░█████    █░█     \t"  "           █████░█░█░█████            ";
      logger<<"        █░█  ██░░░░░░░░░░░░░██  █░█      \t"  "          ██░░░░░█░█░░░░░██           ";
      logger<<"         █░██░░░░░░░░░░░░░░░░░██░█       \t"  "        ██░░░░░░░█░█░░░░░░░██         ";
      logger<<"    ▄▄▄▄▄▄███████████░███████████▄▄▄▄▄▄ \t"  "       ██░░░░░░░░█░█░░░░░░░░██        ";
      logger<<"   █░░░░░░█░████████░░░████████░█░░░░░░█ \t"  "       █░░░░░░░░░█░█░░░░░░░░░█        ";
      logger<<"    ▀▀▀▀▀▀█░░░████░░░░░░░████░░░█▀▀▀▀▀▀ \t"  "       █░░░░░░░░░█░█░░░░░░░░░█        ";
      logger<<"          ██░░░░░░░░░░░░░░░░░░░░█        \t"  "       ██░░░░░░░░█░█░░░░░░░░░█        ";
      logger<<"         █░██░░░░░███████░░░░░░█░█       \t"  "        ██░░░░░░░█░█░░░░░░░░█         ";
      logger<<"        █░█  █░░░░░░░░░░░░░░░██ █░█      \t"  "          █░░░░░░█░█░░░░░░██          ";
      logger<<"       █░█    ██░░░░░░░░░░░██    █░█     \t"  "           ██░░░░█░█░░░░██            ";
      logger<<"      █░█     █░███████████░█     █░█    \t"  "             ███████████              ";
      logger<<"     █░█     █░█    █░█    █░█     █░█   \t"  "                 █░█                  ";
      logger<<"     ▀▀     █░█     █░█     █░█     ▀▀  \t"   "                 █░█                  ";
      logger<<"           █░█      █░█      █░█        \t"   "                 █░█                 ";
      logger<<"          █░█       █░█       █░█       \t"   "                 █░█                 ";
      logger<<"          ▀▀        ▀█▀        ▀▀       \t"  "                  ▀█▀                ";
      logger<<"";
    }
    
    /// Prints the version, and contacts
    void printVersionContacts()
      const
    {
      logger<<"\nInitializing "<<PACKAGE_NAME<<" library, v"<<PACKAGE_VERSION<<", send bug report to <"<<PACKAGE_BUGREPORT<<">";
    }
    
    /// Prints the git info
    void printGitInfo()
      const
    {
      logger<<"Commit "<<GIT_HASH<<" made at "<<GIT_TIME<<" by "<<GIT_COMMITTER<<" with message: \""<<GIT_LOG<<"\"";
    }
    
    /// Prints configure info
    void printConfigurePars()
      const
    {
      logger<<"Configured at "<<CONFIG_TIME<<" with flags: "<<CONFIG_FLAGS<<"";
    }
    
    /// Says bye bye
    void printBailout()
      const
    {
      logger<<"\n Ciao!\n";
    }
    
  public:
    
    /// Creates
    Aliver()
    {
      printBanner();
      printVersionContacts();
      printGitInfo();
      printConfigurePars();
      
      logger<<"ANNA "<<threads.isMasterThread();//  loopSplit(0,10,[](const int& rank,const int& i){printf("Rank %d prints %d\n",rank,i);});
      
      // threads.loopSplit(0,10,[](const int& rank,const int& i){printf("Rank %d prints again %d\n",rank,i);});
    }
    
    /// Destroyer
    ~Aliver()
    {
      printBailout();
    }
  };
  
  Timer timings("Total time",Timer::NO_FATHER,Timer::UNSTOPPABLE);
  Logger logger("/dev/stdout");
  Aliver aliver;
  Logger fakeLogger("/dev/null");
  Mpi mpi;
  ThreadPool threads;
}
