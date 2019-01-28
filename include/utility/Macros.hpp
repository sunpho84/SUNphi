#ifndef _MACROS_HPP
#define _MACROS_HPP

/// \file Macros.hpp
///
/// \brief Header file to define some usefule macro

/// Concatenate two tokens
///
/// Internal implementation
#define _CONCAT2_IMPL(X,Y)			\
  X ## Y

/// Concatenate two tokens
///
///  Wrapper to beat CPP
#define _CONCAT2_WRAP(X,Y)			\
  _CONCAT2_IMPL(X,Y)

/// Concatenate two tokens
///
/// User accessible implementation
#define CONCAT2(X,Y)				\
  _CONCAT2_WRAP(X,Y)

/// Concatenate three tokens
#define CONCAT3(X,Y,Z)				\
  CONCAT(CONCAT2(X,Y),Z)

/////////////////////////////////////////////////////////////////

/// Merges two tokens using a _
#define NAME2(X,Y)				\
  X ## _ ## Y

/// Merges three tokens with two _
#define NAME3(X,Y,Z)				\
  X ## _ ## Y ## _ ## Z

/// Merges four tokens with three _
#define NAME4(X,Y,W,Z)				\
    X ## _ ## Y ## _ ## W ## _ ## Z

namespace SUNphi
{
}

#endif
