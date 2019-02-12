#ifndef _BACKTRACING_HPP
#define _BACKTRACING_HPP

/// \file BackTracing.hpp
///
/// \brief Defines the backtracing function

#include <containers/Vector.hpp>
#include <utility/String.hpp>

namespace SUNphi
{
  /// Decompose the backtrace symbol
  class BackTraceSymbol
  {
  public:
    
    /// Compilation unit
    const std::string compilUnit;
    
    /// Symbol name
    const std::string symbol;
    
    /// Offset
    const std::string offset;
    
    /// Address
    const std::string address;
    
    /// Parse the string
    BackTraceSymbol(const char* str) :
      compilUnit(substrBetweenPos(str,0,std::string(str).find('('))),
      symbol(substrBetween(str,'(','+')),
      offset(substrBetween(str,'+',')')),
      address(substrBetween(str,'[',']'))
    {
    }
  };
  
  /// Gets the backtracing symbols list
  template <int NMAX_REW=128>
  Vector<BackTraceSymbol> getBackTraceList()
  {
    /// Rewinded stack
    void* callstack[NMAX_REW];
    
    /// Gets the stack and number of lines to be rewinded
    int nRew=
      backtrace(callstack,NMAX_REW);
    
    /// Gets the symbols list
    char** strs=
      backtrace_symbols(callstack,nRew);
    
    /// Result
    Vector<BackTraceSymbol> res;
    res.reserve(nRew);
    
    for(int i=0;i<nRew;i++)
      res.emplace_back(strs[i]);
    
    free(strs);
    
    return
      res;
  }
  
  /// Write the list of called routines
  void printBacktraceList();
}

#endif
