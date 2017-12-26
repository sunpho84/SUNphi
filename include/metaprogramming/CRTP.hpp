#ifndef _CRTP_HPP
#define _CRTP_HPP

/// \file CRTP.hpp
///
/// \brief Support for Curiously Recurring Template Pattern

#include <metaprogramming/SwallowSemicolon.hpp>

/// Perform the cast to base class needed for CRTP
///
/// Overload the ~ operator to implement cast to the basic class of a
/// CRTP class
#define PROVIDE_CRTP_CAST_OPERATOR(CLASS)		\
  /*! Cast operator to class CLASS*/			\
CLASS& operator~()					\
  {							\
    return *static_cast<CLASS*>(this);			\
  }							\
							\
 /*! Constant cast operator to class CLASS*/		\
const CLASS& operator~() const				\
  {							\
    return *static_cast<const CLASS*>(this);		\
  }							\
 SWALLOW_SEMICOLON

#endif
