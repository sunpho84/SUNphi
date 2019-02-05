#ifndef _BACKTRACING_HPP
#define _BACKTRACING_HPP

/// \file BackTracing.hpp
///
/// \brief Defines the backtracing function

#include <cstdlib>
#include <execinfo.h>

#include <ios/MinimalLogger.hpp>

namespace SUNphi
{
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
}

#endif
