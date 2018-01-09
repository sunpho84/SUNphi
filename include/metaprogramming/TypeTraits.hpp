#ifndef _TYPETRAITS_HPP
#define _TYPETRAITS_HPP

/// \file TypeTraits.hpp
///
/// \brief Defines convenient adaptation of the std type_traits
/// library, and extensions useful for the expression handling.

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <type_traits>

namespace SUNphi
{
  /// Short name for decltype(auto)
#define DECLAUTO decltype(auto)
  
  /// A bool constant type holding value "true"
  ///
  /// Useful to create SFINAE tests
  using TrueType=std::true_type;
  
  /// A bool constant type holding value "false"
  ///
  /// Useful to create SFINAE tests
  using FalseType=std::false_type;
  
  /////////////////////////////////////////////////////////////////
  
  /// Empty token to be used in macro expecting qualifier
#define NON_CONST_QUALIF			\
  /* */
  
  /// Token to be used in macro indicating "const" keyword
#define CONST_QUALIF				\
  const
  
  /// Empty token to be used in macro expecting the kind of reference
#define NON_REF					\
  /* */
  
  /// Token to be used in macro indicating "l-value" reference
#define LVALUE_REF				\
  &
  
  /// Token to be used in macro indicating "r-value" reference
#define RVALUE_REF				\
  &&
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any constant volatile qualification
  template <typename T>
  using RemoveCV=typename std::remove_cv<T>::type;
  
  /// Returns the type T without any reference
  template <typename T>
  using RemoveReference=typename std::remove_reference<T>::type;
  
  /// Returns the type T without any reference or qualifier
  template <typename T>
  using Unqualified=RemoveCV<RemoveReference<T>>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Useful to enable or disable SFINAE specialization.
  /// Generic (false) case
  template <bool B,          // Boolean constant deciding whether the type is enabled
	    typename T>      // Type to be enabled
  struct _EnableIf
  {
  };
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Useful to enable or disable SFINAE specialization.
  /// True case
  template <typename T> // Type to be enabled
  struct _EnableIf<true,T>
  {
    /// Type enabled
    typedef T type;
  };
  
  /// Defines type T (default to void) if parameter B is true
  ///
  /// Gives visibility to the internal implementation
  template <bool B,            // Boolean constant deciding whether the type is enabled
	    typename T=void>   // Type to be enabled
  using EnableIf=typename _EnableIf<B,T>::type;
  
  /// Defines \c void if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf for \c T=void .
  template <bool B>
  using VoidIf=EnableIf<B>;
  
  /// Defines type T if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf forcing an explicit type.
  template <bool B,          // Boolean constant deciding whether the type is enabled
	    typename T>      // Type to be enabled
  using TypeIf=EnableIf<B,T>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Forward according to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2951.html
  template <class T,
	    class U,
	    class=EnableIf<(std::is_lvalue_reference<T>::value ?
			    std::is_lvalue_reference<U>::value :
			    true) and
			   std::is_convertible <RemoveReference<U>*,
						RemoveReference<T>*>::value>>
  constexpr T&& forw(U&& u)
  {
    return static_cast<T&&>(u);
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Checks if two types are the same
  ///
  /// Default (false) case
  template <typename T1, // First type to be checked
	    typename T2> // Second type to be checked
  static constexpr bool IsSame=false;
  
  /// Checks if two types are the same
  ///
  /// Default (false) case
  template <typename T> // Unique type
  static constexpr bool IsSame<T,T>
  =true;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Provides type T if B is true, or F if is false
  ///
  /// Forward declaration of internal implementation
  template <bool B,
	    typename T,
	    typename F>
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
	    typename F>  // F Type enabled if false
  using Conditional=typename _Conditional<B,T,F>::type;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Returns const or non const T depending on condition
  template <bool B,      // Condition
	    typename T>  // Type
  using ConstIf=Conditional<B,const T,T>;
  
  /// Returns reference of plain type depending on condition
  template <bool B,      // Condition
	    typename T>  // Type
  using RefIf=Conditional<B,T&,T>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Identifies whether Base is a base class of Derived
  template <typename Base,
	    typename Derived>
  constexpr bool IsBaseOf=std::is_base_of<Base,RemoveReference<Derived>>::value;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Static assert if DERIVED does not derive from BASE
#define STATIC_ASSERT_IS_BASE_OF(BASE,DERIVED)				\
  static_assert(IsBaseOf<BASE,DERIVED>,"Error, type not derived from what expected")
  
  /// Forces type Derived to be derived from Base
  template <typename Base,
	    typename Derived>
  struct ConstrainIsBaseOf
  {
    STATIC_ASSERT_IS_BASE_OF(Base,Derived);
    
    /// Type checked
    using type=Derived;
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Static assert if not passing exactly N types
#define STATIC_ASSERT_ARE_N_TYPES(N,UNEXP_PARPACK)			\
  static_assert(N==sizeof...(UNEXP_PARPACK),"Error, expecting a different number of types")
  
  /// Forces types to be in the given number
  template <int N,               // Number of types expected
	    typename...Args>     // Types counted
  struct ConstrainNTypes
  {
    STATIC_ASSERT_ARE_N_TYPES(N,Args);
  };
  
  //////////////////////////////////////////////////////////////////////
  
  /// Identifies whether a type is a floating-point
  template <typename T>
  constexpr bool IsFloatingPoint=std::is_floating_point<T>::value;
  
  /// Static assert if the type T is not a floating-point
#define STATIC_ASSERT_IS_FLOATING_POINT(T)				\
  static_assert(IsFloatingPoint<T>,"Error, type is not a floating point")
  
  /// Forces the type to be a floating-point
  template <typename T>
  struct ConstrainIsFloatingPoint
  {
    STATIC_ASSERT_IS_FLOATING_POINT(T);
  };
  
  //////////////////////////////////////////////////////////////////////
  
  /// Identifies whether a type is an integer-like
  template <typename T>
  constexpr bool IsIntegral=std::is_integral<T>::value;
  
  /// Static assert if the type T is not an integer-like
#define STATIC_ASSERT_IS_INTEGRAL(T)			\
  static_assert(IsIntegral<T>,"Error, type is not an integral")
  
  /// Forces the type to be integer-like
  template <typename T>
  struct ConstrainIsIntegral
  {
    STATIC_ASSERT_IS_INTEGRAL(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Identifies whether a set of types are an integer-like
  template <typename Head=int,
	    typename...Tail>
  constexpr bool AreIntegral=AreIntegral<Head> and AreIntegral<Tail...>;
  
  /// Identifies whether a single types is integer-like
  template <typename T>
  constexpr bool AreIntegral<T>
  =IsIntegral<T>;
  
  /// Static assert if the types T are not an integer-like
#define STATIC_ASSERT_ARE_INTEGRALS(...)			\
  static_assert(AreIntegral<__VA_ARGS__>,"Error, types are not all an integral")
  
  /// Forces the type to be integer-like
  template <typename...Args>
  struct ConstrainAreIntegrals
  {
    STATIC_ASSERT_ARE_INTEGRALS(Args...);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Provides a SFINAE to be used in template par list
  ///
  /// This follows
  /// https://stackoverflow.com/questions/32636275/sfinae-with-variadic-templates
  /// as in this example
  /// \code
  /// template <typename D,
  ///           SFINAE_ON_TEMPLATE_ARG(IsSame<D,int>)>
  /// void foo(D i) {} // fails if D is not int
  /// \endcode
#define SFINAE_ON_TEMPLATE_ARG(__VA_ARGS__)	\
  TypeIf<__VA_ARGS__,void*> =nullptr
  
  /// Provides template par list to unprioritize default SFINAE
  ///
  /// Use as last argument of a function overloaded by a other
  /// implementations using SFINAE to detect the proper version to be
  /// used. This has to be used in conjunction with the other macros
  /// SFINAE_WORSEN_DEFAULT_VERSION_ARGS and
  /// SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK as in this example
  ///
  /// \code
  /// template <typename T,
  ///           SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  /// int tell(T a,SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  /// {
  ///   SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
  ///   return 1;
  /// }
  ///
  /// template <typename T,
  ///           std::enable_if_t<sizeof(T)==4,void*> =nullptr>
  /// decltype(auto) tell(T&& a)
  /// {
  ///   return a+1;
  /// }
  ///
  /// int main()
  /// {
  ///    tell(1); //returns 2
  ///
  ///    return 0;
  /// }
  ///\endcode
#define SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS			\
  typename...DummyTypes                      /* Fake list of types                  */ \
  
  /// Provide empty list of args, used to unprioritize default version
#define SFINAE_WORSEN_DEFAULT_VERSION_ARGS				\
  DummyTypes...     /*< Fake list of args */				\
  
  /// Check that no extra arg is passed
#define SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK	\
  STATIC_ASSERT_ARE_N_TYPES(0,DummyTypes)
  
  //////////////////////////////////////////////////////////////////////
  
  /// Defines a "Base" identifier and checks for it
  ///
  /// Given a TYPE, defines another empty type prefixing its name with
  /// "Base", and defines a check IsTYPE to probe wheter a certain
  /// type inherit from TYPE. All extra arguments can be used to
  /// specify inheritance from other classes
#define DEFINE_BASE_TYPE(TYPE,...)					\
  struct Base ## TYPE __VA_ARGS__ {};					\
									\
  /*! Expression which is true if T inherits from \c BASE ## TYPE */	\
  template<typename T>							\
  constexpr bool Is ## TYPE=IsBaseOf<Base ## TYPE,T>;			\
									\
  /*! Class forcing T to inherits from \c BASE ## TYPE */		\
  template<typename T>							\
  using ConstrainIs ## TYPE=ConstrainIsBaseOf<Base ## TYPE,T>
  
  /////////////////////////////////////////////////////////////////
  
  /// Assert type T to have member TAG
  ///
  /// Note that you need to have defined DEFINE_HAS_MEMBER before
#define STATIC_ASSERT_HAS_MEMBER(TAG,...)				\
  static_assert(hasMember_ ## TAG<__VA_ARGS__>,"Type does not have member " #TAG) \
  
  /// Define a member detecter named hasMember_TAG
  ///
  /// Example:
  ///
  /// \code
  /// DEFINE_HAS_MEMBER(ciccio);
  ///
  /// struct fuffa
  /// {
  ///    int ciccio();
  /// };
  ///
  /// int main()
  /// {
  ///   bool has=hasMember_ciccio(fuffa);
  ///   return 0;
  /// }
  /// \endcode
#define DEFINE_HAS_MEMBER(TAG)						\
  /*! Detect if \c Type has member (variable or method) TAG */		\
  /*!                                                       */		\
  /*! Internal implementation                               */		\
  template <typename Type>						\
  struct HasMember_ ## TAG						\
  {									\
    /*! Internal class of size 1, used if Type has the method */	\
    using Yes=char[1];							\
    									\
    /*! Internal class of size 2 used if Type has not the method */	\
    using No=char[2];							\
    									\
    /*! Internal class which does implement the method TAG */		\
    struct Fallback							\
    {									\
      /*! Member actually implemented */			        \
      int TAG;								\
    };									\
    									\
    /*! This class inherits from both Type and Fallback, so it will  */	\
    /*! certainly have the method TAG, possibly two                  */	\
    struct Derived :							\
      public Type,							\
      public Fallback							\
    {									\
    };									\
    									\
    /*! This type can be instantiated only if the U type is          */ \
    /*! unambiguosly understood.*/					\
    template <typename U,						\
	      U>							\
    struct Check;							\
									\
    /*! Forward definition of test function, taking a pointer to the */	\
    /*! type of TAG as argument, returning No. The instantiation     */	\
    /*! will fail if Base have two member TAG implemented, which     */ \
    /*! means that Type has the member                               */ \
    template <typename U>						\
      static No& test(Check<int Fallback::*,&U::TAG>*);			\
									\
    /*! Forward definition of test function, taking a pointer to the */	\
    /*! type of TAG as argument, returning Yes. The instantiation    */ \
    /*! will work when the other fails, which means that Type does   */ \
    /*! have the member                                              */ \
    template <typename U>						\
      static Yes& test(...);						\
    									\
  public:								\
    /*! Result of the check, comparing the size of return type of    */	\
    /*! test with the size of yes			             */	\
    static constexpr bool result=sizeof(test<Derived>(nullptr))==sizeof(Yes); \
  };									\
									\
  /*! Detect if \c Type has member (variable or method) TAG          */ \
  /*!                                                                */	\
  /*! Uses SFINAE to induce ambiguity in the detection of the member */	\
  template <typename Type>						\
  constexpr bool hasMember_ ## TAG=HasMember_ ## TAG<Type>::result;	\
  									\
  /*! \brief Class forcing T to have a member "TAG" defined */		\
  template <typename T>							\
  struct ConstrainHasMember_ ## TAG					\
  {									\
    STATIC_ASSERT_HAS_MEMBER(TAG,T);					\
  }
  
  /////////////////////////////////////////////////////////////////
}

#endif
