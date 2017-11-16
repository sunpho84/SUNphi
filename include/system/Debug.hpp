#ifndef _DEBUG_HPP
#define _DEBUG_HPP

/// \file Debug.hpp
///
/// \brief Header file for debugging routines

#include <cstdarg>
#include <cstdio>
#include <iostream>

/// Macro to make the crash being more explicit
///
#define CRASH(...)							\
  SUNPhi::internalCrash(__LINE__,__FILE__,__PRETTY_FUNCTION__,__VA_ARGS__)

namespace SUNphi
{
  /// Crash with a detailed message
  ///
  /// At the moment print an error message and gets out, to be
  /// enriched. The variadic function should be replaced with a more
  /// robust variadic-template one
  ///
  template <class...Args>
  void internalCrash(int line,const char *file,const char *func,const char *format,const Args&&...args)
  {
    char buffer[1024];
    vsnprintf(buffer,1024,format,args...);
    
    std::cerr<<"ERROR in function "<<func<<" at line "<<line<<" of file "<<file<<": \""<<buffer<<"\""<<std::endl;
    //print_backtrace_list();
    exit(1);
  }
}

#endif
