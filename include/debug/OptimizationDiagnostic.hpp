#ifndef _OPTIMIZATION_DIAGNOSTIC_HPP
#define _OPTIMIZATION_DIAGNOSTIC_HPP

/// \file OptmizationDiagnostic.hpp
///
/// \brief Useful diagnostic to follow optimization

/// Include a comment in the assembler, recognizable in the compilation
#define ASM_BOOKMARK(COMMENT)					\
  asm("#Bookmark " __FILE__ " " COMMENT)

#endif
