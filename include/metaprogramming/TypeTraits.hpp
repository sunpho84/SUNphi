#ifndef _TYPETRAITS_HPP
#define _TYPETRAITS_HPP

/// \file TypeTraits.hpp
///
/// \brief Defines convenient adaptation of the std type_traits library

#include <type_traits>

namespace SUNphi
{
  /// A bool constant type holding value "true"
  ///
  /// Useful to create SFINAE tests
  ///
  using TrueType=std::true_type;
  
  /// A bool constant type holding value "false"
  ///
  /// Useful to create SFINAE tests
  ///
  using FalseType=std::false_type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any constant volatile qualification
  ///
  template <class T>
  using RemoveCV=typename std::remove_cv<T>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any reference
  ///
  template <class T>
  using RemoveReference=typename std::remove_reference<T>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Useful to enable or disable SFINAE specialization. Directly
  /// taken from std library.
  template <bool B,class T=void>
  using EnableIfT=typename std::enable_if_t<B,T>::type;
  
  /// Defines \c void if parameter B is true
  ///
  /// Explicit specialization of \c EnableIfT for \c T=void .
  template <bool B>
  using VoidIf=EnableIfT<B>;
  
  /// Defines type T if parameter B is true
  ///
  /// Explicit specialization of \c EnableIfT forcing an explicit type.
  //
  template <bool B,class T>
  using TypeIf=EnableIfT<B,T>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Identifies whether Base is a base class of Derived
  ///
  template <class Base,class Derived>
  constexpr bool IsBaseOf=std::is_base_of<Base,RemoveReference<Derived>>::value;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Defines a "Base" identifier and checks for it
  ///
  /// Given a TYPE, defines another empty type prefixing its name with
  /// "Base", and defines a check IsTYPE to probe wheter a certain
  /// type inherit from TYPE
  ///
#define DEFINE_BASE_TYPE(TYPE)				\
  struct Base ## TYPE{};				\
							\
  template<typename T>					\
  constexpr bool Is ## TYPE=IsBaseOf<Base ## TYPE,T>
}

#endif
