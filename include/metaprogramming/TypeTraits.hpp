#ifndef _TYPETRAITS_HPP
#define _TYPETRAITS_HPP

/// \file TypeTraits.hpp
///
/// \brief Defines convenient adaptation of the std type_traits
/// library, and extensions useful for the expression handling.

#include <type_traits>

namespace SUNphi
{
  /// A bool constant type holding value "true"
  ///
  /// Useful to create SFINAE tests
  using TrueType=std::true_type;
  
  /// A bool constant type holding value "false"
  ///
  /// Useful to create SFINAE tests
  using FalseType=std::false_type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any constant volatile qualification
  template <class T>
  using RemoveCV=typename std::remove_cv<T>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any reference
  template <class T>
  using RemoveReference=typename std::remove_reference<T>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Useful to enable or disable SFINAE specialization. Directly
  /// taken from std library.
  template <bool B,class T=void>
  using EnableIf=typename std::enable_if_t<B,T>::type;
  
  /// Defines \c void if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf for \c T=void .
  template <bool B>
  using VoidIf=EnableIf<B>;
  
  /// Defines type T if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf forcing an explicit type.
  template <bool B,class T>
  using TypeIf=EnableIf<B,T>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Checks if two types are the same
  ///
  /// Strait borrowed from std lib
  template <class T1,class T2>
  static constexpr bool IsSame=std::is_same<T1,T2>::value;
  
  /////////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Provides type T if B is true, or F if is false
    ///
    /// Forward declaration of internal implementation
    template <bool B,typename T,typename F>
    struct _Conditional;
    
    /// Provides type T if B is true, or F if is false
    ///
    /// True case of internal implementation
    template <typename T,typename F>
    struct _Conditional<true,T,F>
    {
      /// Internal type (T)
      typedef T type;
    };
    
    /// Provides type T if B is true, or F if is false
    ///
    /// False case of internal implementation
    template <typename T,typename F>
    struct _Conditional<false,T,F>
    {
      /// Internal type (F)
      typedef F type;
    };
  }
  
  /// Provides type T if B is true, or F if is false
  ///
  /// Wraps the internal implementation
  template <bool B,typename T,typename F>
  using Conditional=typename Impl::_Conditional<B,T,F>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Identifies whether Base is a base class of Derived
  template <class Base,class Derived>
  constexpr bool IsBaseOf=std::is_base_of<Base,RemoveReference<Derived>>::value;
  
  /////////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Forces type Derived to be derived from Base
    ///
    /// Helper to internally implement the check through a template class
    template <class Base,class Derived>
    class _ConstraintInherit
    {
      static_assert(IsBaseOf<Base,Derived>,"Error, type not derived from what expected");
    };
  }
  
  /// Forces type Derived to be derived from Base
  ///
  /// Wraps the class defining the check
  template <class Base,class Derived>
  using ConstraintInherit=decltype(Impl::_ConstraintInherit<Base,Derived>());
  
  /// Defines a "Base" identifier and checks for it
  ///
  /// Given a TYPE, defines another empty type prefixing its name with
  /// "Base", and defines a check IsTYPE to probe wheter a certain
  /// type inherit from TYPE
#define DEFINE_BASE_TYPE(TYPE)				\
  struct Base ## TYPE{};				\
							\
  template<typename T>					\
  constexpr bool Is ## TYPE=IsBaseOf<Base ## TYPE,T>;	\
							\
  template<typename T>					\
  using ConstraintIs ## TYPE=ConstraintInherit<Base ## TYPE,T>
}

#endif
