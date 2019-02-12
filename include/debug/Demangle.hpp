#ifndef _DEMANGLE_HPP
#define _DEMANGLE_HPP

/// \file Demangle.hpp
///
/// \brief Demangle symbols using abi

#if __has_include(<cxxabi.h>)
 
 #include <cxxabi.h>
 
 /// Declares demanglability
 #define CAN_DEMANGLE

#endif

#include <utility/String.hpp>

namespace SUNphi
{
  /// Demangle a string
  ///
  /// If the compiler has no abi functionality, the original string is
  /// returned.
  template <typename T>                 // Type of the demangling object
  std::string demangle(const T& what) ///< What to demangle
  {
    
#ifdef CAN_DEMANGLE
  
    /// Returned status of demangle
    int status;
    
    /// Demangled
    char* name=
      abi::__cxa_demangle(cString(what),0,0,&status);
    
    /// Copy the result
    std::string out=
      (status==0)?
      name:
      "(failed demangle)";
    
    // Free if succeded
    if(status==0)
      free(name);
    
    return
      out;
    
#else
    
    return
      "(unable to demangle)";
    
#endif
    
  }
}

#endif
