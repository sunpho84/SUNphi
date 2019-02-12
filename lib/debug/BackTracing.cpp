#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file BackTracing.cpp
///
/// \brief Defines the backtracing function

#include <execinfo.h>

#include <ios/Logger.hpp>

namespace SUNphi
{
  void printBacktraceList()
  {
    runLog<<"Backtracing...";
    
    /// Maximal number of lines to be rewinded
    constexpr int nMaxRew=
      128;
    
    /// Rewinded stack
    void* callstack[nMaxRew];
    
    /// Gets the stack and number of lines to be rewinded
    int nRew=
      backtrace(callstack,nMaxRew);
    
    /// Gets the symbols list
    char** strs=
      backtrace_symbols(callstack,nRew);
    
    for(int i=0;i<nRew;i++)
      runLog<<strs[i];
    
    free(strs);
  }
}
