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
  CONCAT2(CONCAT2(X,Y),Z)

/////////////////////////////////////////////////////////////////

/// Merges two tokens using a _
#define NAME2(X,Y)				\
  CONCAT3(X,_,Y)

/// Merges three tokens with two _
#define NAME3(X,Y,Z)				\
  CONCAT3(X,_,CONCAT3(Y,Z))

/// Merges four tokens with three _
#define NAME4(X,Y,W,Z)				\
  CONCAT3(X,_,CONCAT3(Y,CONVAT3(W,Z)))

namespace SUNphi
{
}

#endif
