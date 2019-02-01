#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

/// \file MinimalLogger.cpp
///
/// \brief Implements the minimalLogger

#include <cstdarg>
#include <cstdio>

#include <ios/Logger.hpp>

namespace SUNphi
{
  void minimalLogger(Logger& logger,
		     const char* format,
		     ...)
  {
    /// Message to be printed
    char mess[256];
    
    /// Starting of the variadic part
    va_list ap;
    
    va_start(ap,format);
    vsnprintf(mess,256,format,ap);
    va_end(ap);
    
    logger<<mess;
  }
}
