#ifndef _MINIMAL_CRASH_HPP
#define _MINIMAL_CRASH_HPP

/// \file MinimalCrash.hpp
///
/// \brief Minimal version of the crasher
///
/// This file can be included in place of Crasher.hpp, in case the
/// former cannot be included due to missing requirements, e.g. in
/// presence of routine from which the actual crasher depends, such as
/// threads or MPI. The actual crasher is called in the cpp file

#include <cstring>
#include <errno.h>

namespace SUNphi
{
  /// Wraps the actual crasher through the old-style C variadic function
  ///
  /// We cannot use variadic template version here, as the crash cannot be called directly here, since "Logger.hpp" is expected
  void minimalCrash(const char* path,        ///< File where the crash happened
		    const int line,          ///< Line number where the crash happened
		    const char* funcName,    ///< Function where the crash happened
		    const char* format,      ///< Formatting string
		    ...);
  
  /// Initialize the minimal crasher
#define MINIMAL_CRASH(...)						\
  minimalCrash(__FILE__,__LINE__,__PRETTY_FUNCTION__,__VA_ARGS__)
  
  /// Minimal crash with stdlib error
#define MINIMAL_CRASH_STDLIBERR(STRING)					\
  MINIMAL_CRASH(__FILE__,__LINE__,__PRETTY_FUNCTION__,STRING " stdliberr: %s",strerror(errno))
}

#endif
