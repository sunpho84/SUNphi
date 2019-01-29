#ifndef _DEBUG_HPP
#define _DEBUG_HPP

/// \file Debug.hpp
///
/// \brief Header file for debugging routines

#include <execinfo.h>
#include <iostream>

/// Macro to make the crash being more explicit
#define CRASH(...)							\
  SUNphi::internalCrash(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)

/// Include a comment in the assembler, recognizable in the compilation
#define ASM_BOOKMARK(COMMENT)					\
  asm("#Bookmark " __FILE__ " " COMMENT)

namespace SUNphi
{
  /// Write the list of called routines
  inline void printBacktraceList()
  {
    void* callstack[128];
    int frames=backtrace(callstack,128);
    char** strs=backtrace_symbols(callstack,frames);
    
    std::cerr<<"Backtracing..."<<std::endl;
    for(int i=0;i<frames;i++)
      std::cerr<<strs[i]<<std::endl;
    
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
