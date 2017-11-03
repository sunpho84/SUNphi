#ifndef _CRTP_HPP
#define _CRTP_HPP

/// \file CRTP.hpp
///
/// \brief Support for Curiously Recurring Template Pattern

/// Perform the cast to base class needed for CRTP
///
/// Overload the ~ operator to implement cast to the basic class of a
/// CRTP class
///
#define DEFINE_CRTP_CAST_OPERATOR(CLASS)		\
  CLASS& operator~()					\
  {							\
    return *static_cast<CLASS*>(this);			\
  }							\
  const CLASS& operator~() const			\
  {							\
    return *static_cast<const CLASS*>(this);		\
  }							\
  CLASS_EAT_SEMICOLON

#endif
