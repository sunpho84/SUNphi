#ifndef _INLINE_HPP
#define _INLINE_HPP

/// \file Inline.hpp
///
/// \brief Force inlining of a function

/// Force inlining of a function
#define ALWAYS_INLINE \
  __attribute__((always_inline)) inline

#endif
