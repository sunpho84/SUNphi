#ifndef _ENABLEIF_HPP
#define _ENABLEIF_HPP

/// \file EnableIf.hpp
///
/// \brief Define constructs derived from EnableIf

#include <type_traits>

namespace SUNphi
{
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Useful to enable or disable SFINAE specialization. Directly
  /// taken from std library.
  ///
  /// Example:
  /// \code
  /// template <class T>
  /// EnableIfT<true,T> test(T in)
  /// {
  ///    return in;
  /// }
  /// \endcode
  ///
  template <bool B,class T=void>
  using EnableIfT=typename std::enable_if_t<B,T>::type;
  
  /// Defines \c void if parameter B is true
  ///
  /// Explicit specialization of \c EnableIfT for \c T=void .
  ///
  /// Example:
  /// \code
  /// template <class T>
  /// VoidIf<true> test(T in)
  /// {
  ///    cout<<in<<endl;
  /// }
  /// \endcode
  ///
  template <bool B>
  using VoidIf=EnableIfT<B>;
  
  /// Defines type T if parameter B is true
  ///
  /// Explicit specialization of \c EnableIfT .
  ///
  /// Example:
  /// \code
  /// template <class T>
  /// EnableIfT<true,T> test(T in)
  /// {
  ///    return in;
  /// }
  /// \endcode
  ///
  template <bool B,class T>
  using TypeIf=EnableIfT<B,T>;
}

#endif
