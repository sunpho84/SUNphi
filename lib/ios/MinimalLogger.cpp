#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file MinimalLogger.cpp
///
/// \brief Implements the minimalLogger, see explenation in the hpp

#include <cstdarg>
#include <cstdio>

#include <ios/Logger.hpp>
#include <ios/MinimalLogger.hpp>

namespace SUNphi
{
  void minimalLogger(Logger& logger,
		     const char* format,
		     ...)
  {
    /// Maximal length to be be printed
    constexpr int MAX_LENGTH=
      256;
    
    /// Message to be printed
    char message[MAX_LENGTH];
    
    /// Starting of the variadic part
    va_list ap;
    va_start(ap,format);
    
    /// Resulting length if the space had been enough
    int rc=
      vsnprintf(message,MAX_LENGTH,format,ap);
    va_end(ap);
    
    /// Check if it was truncated
    bool truncated=
      (rc<0 or rc>=MAX_LENGTH);
    
    if(truncated)
      logger<<message<<" (truncated line)";
    else
      logger<<message;
  }
}
