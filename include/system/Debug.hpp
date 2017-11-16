#ifndef _DEBUG_HPP
#define _DEBUG_HPP

/// \file Debug.hpp
///
/// \brief Header file for debugging routines

#include <cstdio>
#include <iostream>

/// Macro to make the crash being more explicit
///
#define CRASH(...)							\
  SUNphi::internalCrash(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)

namespace SUNphi
{
  /// Variadic print to a stream
  ///
  std::ostream& Print(std::ostream& out)
  {
    return out;
  }
  
  /// Variadic print to a stream
  ///
  template <class Head,class...Tail>
  std::ostream& Print(std::ostream& out,const Head& head,const Tail&...tail)
  {
    out<<head;
    if(sizeof...(tail)) out<<" ";
    return Print(out,tail...);
  }
  
  /// Crash with a detailed message
  ///
  /// At the moment print an error message and gets out, to be
  /// enriched. The variadic function should be replaced with a more
  /// robust variadic-template one
  ///
  template <class...Args>
  void internalCrash(int line,const char *file,const char *func,const Args&...args)
  {
    Print(std::cerr,"ERROR in function ",func," at line ",line," of file ",file,": \"",args...,"\"\n");
    //print_backtrace_list();
    exit(1);
  }
}

#endif
