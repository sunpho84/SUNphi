#ifndef _DEBUG_HPP
#define _DEBUG_HPP

/// \file Debug.hpp
///
/// \brief Header file for debugging routines

#include <execinfo.h>
#include <iostream>

#include <ios/MinimalLogger.hpp>

/// Macro to make the crash being more explicit
#define CRASH(...)							\
  SUNphi::internalCrash(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)

/// Include a comment in the assembler, recognizable in the compilation
#define ASM_BOOKMARK(COMMENT)					\
  asm("#Bookmark " __FILE__ " " COMMENT)

namespace SUNphi
{
  /// Keep all the information to crash
  struct Crasher
  {
    /// File where the crash happened
    const char* path;
    
    /// Line number where the crash happened
    const int line;
    
    /// Function where the crash happened
    const char* funcName;
  };
  
  /// Initialize the crasher
#define CRASHI					\
  runLog<<Crasher{__FILE__,__LINE__,__PRETTY_FUNCTION__}
  
  /// Write the list of called routines
  inline void printBacktraceList()
  {
    void* callstack[128];
    int frames=backtrace(callstack,128);
    char** strs=backtrace_symbols(callstack,frames);
    
    minimalLogger(runLog,"Backtracing...");
    for(int i=0;i<frames;i++)
      minimalLogger(runLog,strs[i]);
    
    free(strs);
  }
  
  /// Crash with a detailed message
  template <class...Args>
  void internalCrash(const int line,        ///< Line of code where the crash happended
		     const char *path,      ///< Name of the file where the crash happened
		     const char *funcName,  ///< Name of the function where the crash happened
		     const Args&...args)    ///< Arguments to print the error
  {
    std::cerr<<"ERROR in function "<<funcName<<" at line "<<line<<" of file "<<path<<": \"";
    ((std::cerr<<" "<<args),...);
    printBacktraceList();
    
    exit(1);
  }
}

#endif
