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
  /// Useful to enable or disable SFINAE specialization.
  /// Generic (false) case
  template <bool B,       // Boolean constant deciding whether the type is enabled
	    class T>      // Type to be enabled
  struct _EnableIf
  {
  };
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Useful to enable or disable SFINAE specialization.
  /// True case
  template <class T> // Type to be enabled
  struct _EnableIf<true,T>
  {
    /// Type enabled
    typedef T type;
  };
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Gives visibility to the internal implementation
  template <bool B,         // Boolean constant deciding whether the type is enabled
	    class T=void>   // Type to be enabled
  using EnableIf=typename _EnableIf<B,T>::type;
  
  /// Defines \c void if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf for \c T=void .
  template <bool B>
  using VoidIf=EnableIf<B>;
  
  /// Defines type T if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf forcing an explicit type.
  template <bool B,       // Boolean constant deciding whether the type is enabled
	    class T>      // Type to be enabled
  using TypeIf=EnableIf<B,T>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Checks if two types are the same
  ///
  /// Default (false) case
  template <class T1, // First type to be checked
	    class T2> // Second type to be checked
  static constexpr bool IsSame=false;
  
  /// Checks if two types are the same
  ///
  /// Default (false) case
  template <class T> // Unique type
  static constexpr bool IsSame<T,T>
  =true;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Provides type T if B is true, or F if is false
  ///
  /// Forward declaration of internal implementation
  template <bool B,typename T,typename F>
  struct _Conditional;
  
  /// Provides type T if B is true, or F if is false
  ///
  /// True case of internal implementation
  template <typename T,
	    typename F>
  struct _Conditional<true,T,F>
  {
    /// Internal type (T)
    typedef T type;
  };
  
  /// Provides type T if B is true, or F if is false
  ///
  /// False case of internal implementation
  template <typename T,
	    typename F>
  struct _Conditional<false,T,F>
  {
    /// Internal type (F)
    typedef F type;
  };
    
  /// Provides type T if B is true, or F if is false
  ///
  /// Wraps the internal implementation
  template <bool B,      // Condition enabling one or the other type
	    typename T,  // T Type enabled if true
	    typename F>  //F Type enabled if false
  using Conditional=typename _Conditional<B,T,F>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Identifies whether Base is a base class of Derived
  template <class Base,
	    class Derived>
  constexpr bool IsBaseOf=std::is_base_of<Base,RemoveReference<Derived>>::value;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Static assert if DERIVED does not derive from BASE
#define STATIC_ASSERT_IF_NOT_BASE_OF(BASE,DERIVED)			\
  static_assert(IsBaseOf<BASE,DERIVED>,"Error, type not derived from what expected")
  
  /// Forces type Derived to be derived from Base
  template <class Base,
	    class Derived>
  class ConstraintIsBaseOf
  {
    STATIC_ASSERT_IF_NOT_BASE_OF(Base,Derived);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Static assert if not passing exactly N types
#define STATIC_ASSERT_IF_NOT_N_TYPES(N,UNEXP_PARPACK)				\
  static_assert(N==sizeof...(UNEXP_PARPACK),"Error, expecting a different number of types")
  
  /// Forces types to be in the given number
  template <int N,
	    class...Args>
  class ConstraintNTypes
  {
    STATIC_ASSERT_IF_NOT_N_TYPES(N,Args);
  };
  
  //////////////////////////////////////////////////////////////////////
  
  /// Identifies whether a type is a floating-point
  template <class T>
  constexpr bool IsFloatingPoint=std::is_floating_point<T>::value;
  
  /// Static assert if the type T is not a floating-point
#define STATIC_ASSERT_IF_NOT_FLOATING_POINT(T)				\
  static_assert(IsFloatingPoint<T>,"Error, type is not a floating point")
  
  /// Forces the type to be a floating-point
  template <class T>
  class ConstraintIsFloatingPoint
  {
    STATIC_ASSERT_IF_NOT_FLOATING_POINT(T);
  };
  
  //////////////////////////////////////////////////////////////////////
  
  /// Identifies whether a type is an integer-like
  template <class T>
  constexpr bool IsIntegral=std::is_integral<T>::value;
  
  /// Static assert if the type T is not an integer-like
#define STATIC_ASSERT_IF_NOT_INTEGRAL(T)			\
  static_assert(IsIntegral<T>,"Error, type is not an integral")
  
  /// Forces the type to be integer-like
  template <class T>
  class ConstraintIsIntegral
  {
    STATIC_ASSERT_IF_NOT_INTEGRAL(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Identifies whether a set of types are an integer-like
  template <class Head=int,
	    class...Tail>
  constexpr bool AreIntegral=AreIntegral<Head> and AreIntegral<Tail...>;
  
  /// Identifies whether a single types is integer-like
  template <class T>
  constexpr bool AreIntegral<T>
  =IsIntegral<T>;
  
  /// Static assert if the types T are not an integer-like
#define STATIC_ASSERT_IF_NOT_INTEGRALS(...)			\
  static_assert(AreIntegral<__VA_ARGS__>,"Error, types are not all an integral")
  
  /// Forces the type to be integer-like
  template <class...Args>
  class ConstraintAreIntegrals
  {
    STATIC_ASSERT_IF_NOT_INTEGRALS(Args...);
  };
  
  //////////////////////////////////////////////////////////////////////
  
  /// Defines a "Base" identifier and checks for it
  ///
  /// Given a TYPE, defines another empty type prefixing its name with
  /// "Base", and defines a check IsTYPE to probe wheter a certain
  /// type inherit from TYPE
#define DEFINE_BASE_TYPE(TYPE)						\
  struct Base ## TYPE{};						\
									\
  /*! Expression which is true if T inherits from \c BASE ## TYPE */	\
  template<typename T>							\
  constexpr bool Is ## TYPE=IsBaseOf<Base ## TYPE,T>;			\
									\
  /*! Class  forcing T to inherits from \c BASE ## TYPE */		\
  template<typename T>							\
  using ConstraintIs ## TYPE=ConstraintIsBaseOf<Base ## TYPE,T>
}

#endif
