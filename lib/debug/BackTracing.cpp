#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file BackTracing.cpp
///
/// \brief Defines the backtracing function

#include <execinfo.h>

#include <debug/BackTracing.hpp>
#include <ios/Logger.hpp>

namespace SUNphi
{
  void printBacktraceList()
  {
    runLog()<<"Backtracing...";
    
    for(auto &p : getBackTraceList())
      runLog()<<p;
  }
}
