#ifndef _INLINE_HPP
#define _INLINE_HPP

/// \file Inline.hpp
///
/// \brief Header file for the definition of inline-enforcing

#define ALWAYS_INLINE				\
  __attribute__((always_inline)) inline

#endif