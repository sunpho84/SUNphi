#ifndef _UNUSED_HPP
#define _UNUSED_HPP

/// \file Unused.hpp
///
/// \brief Header file to suppress "Unused Variable" warning

/// Dummy function to be called to suppress "Unused Variable" warning
///
/// Example:
/// \code
///   const int a=10;
///   maybeUnused(a);
/// \endcode
template<typename T>
inline void maybeUnused(const T&) ///< Variable to be suppressed
{
}

/// Suppress the "Unused Variable" warning at any scope
#define MAYBE_UNUSED(A)							\
  /*! Forward declaration of a dummy function, used to suppress "Unused variable" warning */ \
  /*! \return the type of A */						\
  Conditional<0,decltype(maybeUnused(A)),void> maybeUnusedFD()

#endif
