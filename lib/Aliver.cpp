#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file Aliver.cpp
///
/// \brief Starts the library and close it

#include <gitInfo.hpp>
#include <system/MpiImpl.hpp>

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

