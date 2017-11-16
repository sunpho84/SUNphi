#ifndef _DEBUG_HPP
#define _DEBUG_HPP

/// \file Debug.hpp
///
/// \brief Header file for debugging routines

#include <execinfo.h>
#include <iostream>

#include <ios/Print.hpp>

/// Macro to make the crash being more explicit
///
#define CRASH(...)							\
  SUNphi::internalCrash(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)

namespace SUNphi
{
  /// Write the list of called routines
  ///
  inline void printBacktraceList()
  {
    void *callstack[128];
    int frames=backtrace(callstack,128);
    char **strs=backtrace_symbols(callstack,frames);
    
    std::cerr<<"Backtracing..."<<std::endl;
    for(int i=0;i<frames;i++) std::cerr<<strs[i]<<std::endl;
    
    free(strs);
  }
  
  /// Crash with a detailed message
  ///
  /// At the moment print an error message and gets out, to be
  /// enriched. The variadic function should be replaced with a more
  /// robust variadic-template one
  ///
  template <class...Args>
  void internalCrash(const int line,const char *path,const char *funcName,const Args&...args)
  {
    print(std::cerr,"ERROR in function ",funcName," at line ",line," of file ",path,": \"",args...,"\"\n");
    printBacktraceList();
    
    exit(1);
  }
}

#endif
