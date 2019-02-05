/// \file Debug.hpp
///
/// \brief Header file for debugging routines

#include <execinfo.h>
#include <iostream>

#include <ios/MinimalLogger.hpp>
#include <metaprogramming/UniversalReferences.hpp>

/// Include a comment in the assembler, recognizable in the compilation
#define ASM_BOOKMARK(COMMENT)					\
  asm("#Bookmark " __FILE__ " " COMMENT)
