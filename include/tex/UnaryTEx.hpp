#ifndef _UNARYTEX_HPP
#define _UNARYTEX_HPP

/// \file UnaryTEx.hpp
///
/// \brief Header file defining basic properties of Unary Template EXpressions

#include <tex/BaseTEx.hpp>

namespace SUNphi
{
  /// Defines the UnaryTEx type traits
  DEFINE_BASE_TYPE(UnaryTEx,: public BaseTEx);
  
  // Defines the check for a member type "ref"
  DEFINE_HAS_MEMBER(ref);
  
  /// Defines the check for a Unary TEx
#define STATIC_ASSERT_IS_UNARY_TEX(...)			\
  STATIC_ASSERT_IS_TEX(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(ref,__VA_ARGS__)
  
  /// Unary TEmplate Expression
  template <typename T>
  struct UnaryTEx :
    public TEx<T>,
    public BaseUnaryTEx
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Set the assignability according to the reference
#define ASSIGNABLE_ACCORDING_TO_REF					\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx can be assigned according to the reference */,RemoveReference<decltype(ref)>::isAssignable)
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking a reference
  ///
  /// The type is needed because we have chosen dedicated name for
  /// each UnaryTEx
#define PROVIDE_UNARY_TEX_SIMPLE_CREATOR(UNARY_TEX,REF_TYPE)	\
  /*! Constructor taking a universal reference */		\
  UNARY_TEX(REF_TYPE&& ref) : ref(ref)				\
  {								\
  }								\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Set aliasing according to the isAliasing of reference
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_REF			\
  /*! Forward aliasing check to the reference */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    return ref.isAliasing(alias);			\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Set aliasing according to a passed pointer (provided class member)
#define IS_ALIASING_ACCORDING_TO_POINTER(_p)				\
  /*! Check the aliasing with reference */				\
  template <typename Tref>						\
  bool isAliasing(const Tref& alias) const				\
  {									\
    const void* pAlias=static_cast<const void*>(&alias);		\
    const void* p=static_cast<const void*>(_p);				\
									\
    return pAlias==p;							\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Create a simple builder with a name and a UNARY_TEX returned type
#define SIMPLE_UNARY_TEX_BUILDER(BUILDER,UNARY_TEX)			\
  /*! Simple UNARY_TEX builder called BUILDER */			\
  /*!                                         */			\
  /*! Plain UNARY_TEX getting a plain TEx     */			\
  template <typename T>              /* Type of the TEx to get */	\
  DECLAUTO BUILDER(T&& ref)          /*!< Quantity to act upon */	\
  {									\
    /* cout<<"Constructing a UNARY_TEX for type "<<T::name()<<endl; */	\
    return UNARY_TEX<T>(forw<T>(ref));					\
  }
  
  
  /// Provides the evaluator with or without const attribute
#define PROVIDE_CONST_OR_NON_UNARY_TEX_DEFAULT_EVALUATOR(T,QUALIFIER)	\
  /*! QUALIFIER Evaluator for type T */					\
  template <class...Args>						\
  friend decltype(auto) eval(QUALIFIER T& EXP,    /*!< Expression to eval */ \
			     const Args&...args)  /*!< Parameters to pass */ \
  {									\
    STATIC_ASSERT_ARE_N_TYPES(TK::nTypes,args);				\
    return eval(EXP.ref,forw<QUALIFIER Args>(args)...);			\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Provides a simple evaluator (const and non-const)
  ///
  /// We need to specify the type because we chose dedicated type for
  /// each TEx
#define PROVIDE_UNARY_TEX_DEFAULT_EVALUATOR(T)				\
  PROVIDE_CONST_OR_NON_UNARY_TEX_DEFAULT_EVALUATOR(T,/**/);		\
  PROVIDE_CONST_OR_NON_UNARY_TEX_DEFAULT_EVALUATOR(T,const);		\
  
  /// Implements a duplicated-call canceller
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// conj(conj(cicc)); // returns cicc
  /// \endcode
#define CANCEL_DUPLICATED_UNARY_TEX_CALL(CALLER,UNARY_TEX)		\
  /*! Simplify CALLER(UNARY_TEX) expression */				\
  /*!                                      */				\
  /*! Returns the nested reference         */				\
  template <typename T>                /* Type of the nested UNARY_TEX */ \
  DECLAUTO CALLER(UNARY_TEX<T>&& ref)  /*!< Quantity to un-nest       */ \
  {									\
    return forw<T>(ref.ref);						\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Implements a duplicated-call absorber
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// wrap(wrap(cicc)); // returns wrap(cicc)
  /// \endcode
#define ABSORB_DUPLICATED_UNARY_TEX_CALL(CALLER,UNARY_TEX)		\
  /*! Simplify CALLER(UNARY_TEX) expression */				\
  /*!                                      */				\
  /*! Returns the reference               */				\
  template <typename T>                /* Type of the nested UNARY_TEX */ \
  DECLAUTO CALLER(UNARY_TEX<T>&& ref)  /*!< Quantity to absorb         */ \
  {									\
    return forw<UNARY_TEX<T>>(ref);						\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
/// Defines a simple way to deal with nesting
#define UNARY_TEX_GOES_INSIDE(EXT_FUN,UNARY_TEX,INT_FUN)		\
  /*! Simplify EXT_FUN(UNARY_TEX\<T> u) expression */			\
  /*!                                             */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))             */			\
  template <typename T>                 /* Type of the nested UNARY_TEX */ \
  DECLAUTO EXT_FUN(UNARY_TEX<T>&& ref)  /*!< Quantity to call           */ \
  {									\
    return INT_FUN(EXT_FUN(forw<T>(ref.ref)));				\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
}

#endif
