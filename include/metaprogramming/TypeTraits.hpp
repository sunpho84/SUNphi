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
#include <utility>

#include <metaprogramming/SwallowSemicolon.hpp>

namespace SUNphi
{
  /// A bool constant type holding value "true"
  ///
  /// Useful to create SFINAE tests
  using TrueType=
    std::true_type;
  
  /// A bool constant type holding value "false"
  ///
  /// Useful to create SFINAE tests
  using FalseType=
    std::false_type;
  
  /////////////////////////////////////////////////////////////////
  
  /// Empty token to be used in macro expecting qualifier
#define NON_CONST_QUALIF			\
  
  /// Token to be used in macro indicating "const" keyword
#define CONST_QUALIF \
  const
  
  /// Empty token to be used in macro expecting the kind of reference
#define NON_REF
  
  /// Token to be used in macro indicating "l-value" reference
#define LVALUE_REF				\
  &
  
  /// Token to be used in macro indicating "r-value" reference
#define RVALUE_REF				\
  &&
  
  /////////////////////////////////////////////////////////////////
  
  /// Delete the copy-constructor
#define FORBID_CONSTRUCT_BY_COPY(CLASS)		\
  CLASS(const CLASS&)=				\
    delete
  
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
  using EnableIf=
    typename _EnableIf<B,T>::type;
  
  /// Defines \c void if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf for \c T=void .
  template <bool B>
  using VoidIf=
    EnableIf<B>;
  
  /// Defines type T if parameter B is true
  ///
  /// Explicit specialization of \c EnableIf forcing an explicit type.
  template <bool B,          // Boolean constant deciding whether the type is enabled
	    typename T>      // Type to be enabled
  using TypeIf=
    EnableIf<B,T>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Checks if a type is a class
  template <typename T> // Type to check
  [[ maybe_unused ]]
  static constexpr bool isClass=
    std::is_class_v<T>;
  
  /// Checks if a type can be cast into another one
  template <typename From, // Source type
	    typename To>   // Destination type
  [[ maybe_unused ]]
  static constexpr bool canBeConverted=
    std::is_convertible_v<From,To>;
  
  /// Checks if an object can be trivially copied
  template <typename T> // Type to check
  [[ maybe_unused ]]
  static constexpr bool isTriviallyCopyable=
    std::is_trivially_copyable_v<T>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Checks if two types are the same
  ///
  /// Default (false) case
  template <typename T1, // First type to be checked
	    typename T2> // Second type to be checked
  [[ maybe_unused ]]
  static constexpr bool isSame=
    false;
  
  /// Checks if two types are the same
  ///
  /// True case
  template <typename T> // Specialize with a unique type
  static constexpr bool isSame<T,T> =
    true;
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns true if all types Ts are the same
  ///
  /// Internal implementation
  template <typename T=void,
	    typename...Tail>
  constexpr bool _areSame()
  {
    if constexpr(sizeof...(Tail)==0)
       return true;
    else
      return (isSame<T,Tail> && ...);
  }
  
  /// Returns true if all Ts are the same type
  ///
  /// Gives visibility to internal representation
  template <typename...Ts>
  [[ maybe_unused ]]
  constexpr bool areSame=
    _areSame<Ts...>();
  
  /// Assert if types are not the same
#define STATIC_ASSERT_ARE_SAME(...)		\
  static_assert(areSame<__VA_ARGS__>,"Error, types are not the same")
  
  /// Forces a set of types to be the same
  template <class...Args>
  class ConstrainAreSame
  {
    STATIC_ASSERT_ARE_SAME(Args...);
  };
  
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
  using Conditional=
    typename _Conditional<B,T,F>::type;
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any reference
  template <typename T>
  using RemRef=
    typename std::remove_reference<T>::type;
  
  /// Check if the type is lvalue reference
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isLvalue=
    std::is_lvalue_reference<T>::value;
  
  /// Check if the type is rvalue reference
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isRvalue=
    std::is_rvalue_reference<T>::value;
  
  /// Returns reference of plain type depending on condition
  template <bool B,                          // Condition
	    typename _T,                     // Type
	    typename T=RemRef<_T>>  // Remove reference from type
  using RefIf=
    Conditional<B,T&,T>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the type T without any constant volatile qualification
  template <typename T>
  using RemoveCV=
    typename std::remove_cv<T>::type;
  
  /// Returns whether T is const or not
  ///
  /// Warning!!! This is not the same than asking \c std::is_const
  template <typename _T,            // Asked type
	    typename T=RemRef<_T>>  // Type without ref
  [[ maybe_unused ]]
  constexpr bool isConst=
    not isSame<RemoveCV<T>,T>;
  
  /// Returns const or non const T depending on condition
  template <bool B,       // Condition
	    typename _T,  // Type
	    typename T=RemoveCV<_T>>  // Remove const from type
  using ConstIf=
    Conditional<B,const T,T>;
  
  /// Returns the type T without any reference or qualifier
  template <typename T>
  using Unqualified=
    RemoveCV<RemRef<T>>;
  
  /////////////////////////////////////////////////////////////////////
  
  /// Identifies whether Base is a base class of Derived
  template <typename Base,     // The type that can be base
	    typename Derived>  // The type where Base is searched
  [[ maybe_unused]]
  constexpr bool isBaseOf=
    std::is_base_of<Base,RemRef<Derived>>::value;
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns whether the passed argument is a function
  template <typename F>
  [[ maybe_unused ]]
  constexpr bool isFunction=
    std::is_function_v<F>;
  
  /// Returns a pointer-version of F
  template <typename F>
  using AddPointer=
    std::add_pointer_t<F>;
  
  /// Returns F or F* if F is not a function
  template <typename F>
  using AddPointerIfFunction=
    Conditional<isFunction<F>,
		AddPointer<F>,
		F>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Remove \c const qualifier from anything
  ///
  /// \todo  Check that  the  "dangling  reference forbidding"  below,
  /// currently not working, is not actually necessary
  template <typename T>
  constexpr T& asMutable(const T& v) noexcept
  {
    return const_cast<T&>(v);
  }
  
  // /// Forbids a dangling reference
  // template <typename T>
  // void asMutable(const T&&)=delete;
  
  /// Call a const method removing any const qualifier
#define CALL_CLASS_CONST_METHOD_REMOVING_CONST(...)	\
  asMutable(std::as_const(*this).__VA_ARGS__)
  
  /// Provides also a non-const version of the method \c NAME
  ///
  /// See
  /// https://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
  /// A const method NAME must be already present Example
  ///
  /// \code
  // class ciccio
  /// {
  ///   double e{0};
  ///
  /// public:
  ///
  ///   const double& get() const
  ///   {
  ///     return e;
  ///   }
  ///
  ///   PROVIDE_ALSO_NON_CONST_METHOD(get);
  /// };
  /// \endcode
#define PROVIDE_ALSO_NON_CONST_METHOD(NAME)				\
  /*! Overload the \c NAME const method passing all args             */ \
  template <typename...Ts> /* Type of all arguments                  */	\
  DECLAUTO NAME(Ts&&...ts) /*!< Arguments                            */	\
  {									\
    return								\
      CALL_CLASS_CONST_METHOD_REMOVING_CONST(NAME(forw<Ts>(ts)...));	\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
  /// Static assert if DERIVED does not derive from BASE
#define STATIC_ASSERT_IS_BASE_OF(BASE,DERIVED)				\
  static_assert(isBaseOf<BASE,DERIVED>,"Error, type not derived from what expected")
  
  /// Static assert if DERIVED does derive from BASE
#define STATIC_ASSERT_IS_NOT_BASE_OF(BASE,DERIVED)				\
  static_assert(not isBaseOf<BASE,DERIVED>,"Error, type derived from what not expected")
  
  /// Forces type Derived to be derived from Base
  template <typename Base,
	    typename Derived>
  struct ConstrainIsBaseOf
  {
    STATIC_ASSERT_IS_BASE_OF(Base,Derived);
    
    /// Type checked
    using type=
      Derived;
  };
  
  /// Forces type Derived not to be derived from Base
  template <typename Base,
	    typename Derived>
  struct ConstrainIsNotBaseOf
  {
    STATIC_ASSERT_IS_NOT_BASE_OF(Base,Derived);
    
    /// Type checked
    using type=
      Derived;
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
  [[ maybe_unused ]]
  constexpr bool isFloatingPoint=std::is_floating_point<T>::value;
  
  /// Static assert if the type T is not a floating-point
#define STATIC_ASSERT_IS_FLOATING_POINT(T)				\
  static_assert(isFloatingPoint<T>,"Error, type is not a floating point")
  
  /// Forces the type to be a floating-point
  template <typename T>
  struct ConstrainIsFloatingPoint
  {
    STATIC_ASSERT_IS_FLOATING_POINT(T);
  };
  
  //////////////////////////////////////////////////////////////////////
  
  /// Identifies whether a type is an integer-like
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isIntegral=std::is_integral<T>::value;
  
  /// Static assert if the type T is not an integer-like
#define STATIC_ASSERT_IS_INTEGRAL(T)			\
  static_assert(isIntegral<T>,"Error, type is not an integral")
  
  /// Forces the type to be integer-like
  template <typename T>
  struct ConstrainIsIntegral
  {
    STATIC_ASSERT_IS_INTEGRAL(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Identifies whether a set of types are a integer-like
  template <typename Head=int,
	    typename...Tail>
  [[ maybe_unused ]]
  constexpr bool areIntegrals=
    areIntegrals<Head> and areIntegrals<Tail...>;
  
  /// Identifies whether a single types is integer-like
  template <typename T>
  constexpr bool areIntegrals<T> =
    isIntegral<T>;
  
  /// Static assert if the types T are not an integer-like
#define STATIC_ASSERT_ARE_INTEGRALS(...)			\
  static_assert(areIntegrals<__VA_ARGS__>,"Error, types are not all an integral")
  
  /// Forces the type to be integer-like
  template <typename...Args>
  struct ConstrainAreIntegrals
  {
    STATIC_ASSERT_ARE_INTEGRALS(Args...);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Provides an EnableIf with the given name for a certain condition
  ///
  /// Must be declared with deduced type to void template parameters
  /// that cannot be deduced
#define PROVIDE_ENABLE_IF_FOR(TYPE,					\
			      ...)					\
  /*! Provides the class itself if T satisfies the condition */		\
  template <typename T,							\
	    typename Ret=void>						\
  using EnableIfIs ## TYPE=						\
    EnableIf<__VA_ARGS__,Ret>
  
  /// Provides an EnableIf with the given name if isTYPE exists
#define PROVIDE_ENABLE_IF_FOR_IS_TYPE(TYPE)				\
  /*! Provides the class itself if T is of the given type */		\
  PROVIDE_ENABLE_IF_FOR(TYPE,is ## TYPE<T>)
  
  /// Provides an EnableIf with the given type
#define PROVIDE_ENABLE_IF_FOR_TYPE(TYPE)				\
  /*! Provides the class itself if T is of the given type */		\
  PROVIDE_ENABLE_IF_FOR(TYPE,isSame<T,TYPE>)
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a "Base" identifier and checks for it
  ///
  /// Given a TYPE, defines another empty type prefixing its name with
  /// "Base", and defines a check IsTYPE to probe wheter a certain
  /// type inherit from TYPE. All extra arguments can be used to
  /// specify inheritance from other classes
#define DEFINE_BASE_TYPE(TYPE,...)					\
  struct Base ## TYPE __VA_ARGS__ {};					\
									\
  /*! Expression which is true if T inherits from \c Base ## TYPE */	\
  template <typename T>							\
  [[ maybe_unused ]]							\
  constexpr bool is ## TYPE=						\
    isBaseOf<Base ## TYPE,T>;						\
									\
  PROVIDE_ENABLE_IF_FOR_IS_TYPE(TYPE);					\
									\
  /*! Class forcing T to inherits from \c Base ## TYPE */		\
  template <typename T>							\
  using ConstrainIs ## TYPE=						\
    ConstrainIsBaseOf<Base ## TYPE,T>;					\
									\
  /*! Class forcing T not to inherits from \c Base ## TYPE */		\
  template <typename T>							\
  using ConstrainIsNot ## TYPE=						\
    ConstrainIsNotBaseOf<Base ## TYPE,T>;				\
									\
  /*! Static assert if the types do not inherit from \c Base ## TYPE */	\
  template <typename...Args>						\
  struct ConstrainAre ## TYPE ## s					\
  {									\
    static_assert((is ## TYPE<Args> && ...),				\
		  "Error, types are not all " #TYPE);			\
  }
  
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
    static constexpr bool result=					\
      sizeof(test<Derived>(nullptr))==sizeof(Yes);			\
  };									\
									\
  /*! Intemediate function to distinguish the non-class case */		\
  template <typename Type>						\
  [[ maybe_unused ]]							\
  constexpr bool hasMember_ ## TAG ## Helper()				\
  {									\
    if constexpr(isClass<Type>)						\
      return						                \
	HasMember_ ## TAG<Type>::result;			        \
    else								\
      return								\
	false;								\
  }									\
  									\
  /*! Detect if \c Type has member (variable or method) TAG          */ \
  /*!                                                                */	\
  /*! Uses SFINAE to induce ambiguity in the detection of the member */	\
  template <typename Type>						\
  [[ maybe_unused ]]							\
  constexpr bool hasMember_ ## TAG=					\
    hasMember_ ## TAG ## Helper<Type>();				\
  									\
  /*! \brief Class forcing T to have a member "TAG" defined */		\
  template <typename T>							\
  struct ConstrainHasMember_ ## TAG					\
  {									\
    STATIC_ASSERT_HAS_MEMBER(TAG,T);					\
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Check that the binary operator exists
#define DEFINE_BINARY_OPERATOR_IMPLEMENTATION_CHECK(CHECK_NAME,		\
						    STRUCT_NAME,	\
						    OPERATOR)		\
  /*! Structure used to check if the operator is implemented */		\
  template <typename S,							\
	    typename T>							\
  struct STRUCT_NAME							\
  {									\
    /*! Path followed when the operator is implemented */		\
    template <typename U,						\
	      typename V>						\
      static auto test(U*)->decltype(std::declval<U>()			\
				     OPERATOR				\
				     std::declval<V>());		\
    									\
    /*! Default case in which the binary operation cannot be performed */ \
    template <typename,							\
	      typename>							\
      static auto test(...)->std::false_type;				\
    									\
    /*! Result of the check */						\
    static constexpr bool res=						\
      not isSame<std::false_type,decltype(test<S,T>(0))>;		\
  };									\
  									\
  /*! Check that operator OPERATOR is implemented */			\
  template <typename S,							\
	    typename T>							\
  [[ maybe_unused ]]							\
  constexpr bool CHECK_NAME=						\
    STRUCT_NAME<S,T>::res
  
  DEFINE_BINARY_OPERATOR_IMPLEMENTATION_CHECK(canPrint,CanPrint,<<);
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the signed version of the passed type
  template <typename T>     // Type to unsign
  using SignedOf=
    RefIf<isLvalue<T>,std::make_signed_t<RemRef<T>>>;
  
  /// Returns the unsigned version of the passed type
  template <typename T>     // Type to sign
  using UnsignedOf=
    RefIf<isLvalue<T>,std::make_unsigned_t<RemRef<T>>>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Provides a check returning whether the class is of a given kind
#define DEFINE_IS_THE_TEMPLATED_CLASS(CLASS)				\
  /*! Check if the class is a CLASS              */			\
  /*!                                            */			\
  /*! False case                                 */			\
  template <typename T>							\
  constexpr bool _is ## CLASS(const T*)					\
  {									\
    return								\
      false;								\
  }									\
  									\
  /*! Check if the class is a CLASS              */			\
  /*!                                            */			\
  /*! True case                                  */			\
  template <typename...Ts>						\
  constexpr bool _is ## CLASS(const CLASS<Ts...>*)			\
  {									\
    return								\
      true;								\
  }									\
									\
  /*! Check if the class is a CLASS              */			\
  /*!                                            */			\
  /*! Calls the internal implementation          */			\
  template <typename T>							\
  [[ maybe_unused ]]							\
  constexpr bool is ## CLASS=						\
     _is ## CLASS((RemRef<T>*)nullptr);                                 \
									\
  PROVIDE_ENABLE_IF_FOR_IS_TYPE(CLASS)
}

#endif
