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
  
  /// Provide the reference to the object
#define PROVIDE_UNARY_TEX_REF(REF_TYPE)		\
  /*! Reference to the object */		\
  Reference<REF_TYPE> ref
  
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
  explicit UNARY_TEX(REF_TYPE&& tex) : ref(tex)			\
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
#define SIMPLE_UNARY_TEX_BUILDER(BUILDER,   /*!< Name of builder fun           */ \
				 UNARY_TEX) /*!< Name of the UnaryTex to build */ \
  /*! Simple UNARY_TEX builder called BUILDER */			\
  /*!                                         */			\
  /*! Plain UNARY_TEX getting a plain TEx     */			\
  template <typename T, 	    /* Type of the TEx to get       */	\
	    typename...Args>						\
  UNARY_TEX<T> BUILDER(T&& tex,     /*!< TEx to act upon            */	\
		       Args...)						\
  {									\
    /* cout<<"Constructing a UNARY_TEX for type "<<T::name()<<endl; */	\
    return UNARY_TEX<T>(forw<T>(tex));					\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Provides the evaluator
#define PROVIDE_UNARY_TEX_DEFAULT_EVALUATOR(UNARY_TEX)			\
  /*! QUALIFIER Evaluator for expression UNARY_TEX */			\
  template <typename D,							\
	    typename...Args>						\
  friend DECLAUTO eval(D&& exp,            /*!< Expression to eval */	\
		       const Args&...args, /*!< Parameters to pass */	\
		       TypeIf<Is ## UNARY_TEX<D>,void*> =nullptr,	\
		       ...)						\
  {									\
    STATIC_ASSERT_ARE_N_TYPES(TK::nTypes,args);				\
    return eval(exp.ref,forw<const Args>(args)...);			\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Implements a duplicated-call canceller
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// conj(conj(cicc)); // returns cicc
  /// \endcode
#define CANCEL_DUPLICATED_UNARY_TEX_CALL(CALLER,    /*!< Name of builder */ \
					 UNARY_TEX) /*!< Type to un-nest */ \
  /*! Simplify CALLER(UNARY_TEX) expression */				\
  /*!                                       */				\
  /*! Returns the nested reference          */				\
  template <typename T,                              /* Type of the expression                  */ \
	    typename=EnableIf<Is ## UNARY_TEX<T>>>   /* Enable only if tex is already UNARY_TEX */ \
  DECLAUTO CALLER(T&& tex)	/*!< Quantity to un-nest   */		\
  {									\
    return tex.ref;							\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Implements a duplicated-call absorber
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// wrap(wrap(cicc)); // returns wrap(cicc)
  /// \endcode
#define ABSORB_DUPLICATED_UNARY_TEX_CALL(CALLER,     /*!< Name of builder                */ \
					 UNARY_TEX)  /*!< Type to absorb                 */ \
  /*! Simplify CALLER(UNARY_TEX) expression */				\
  /*!                                       */				\
  /*! Returns the reference                 */				\
  /*! \todo enforce const                   */				\
  template <typename D,                                   /* Type of the nested UNARY_TEX           */ \
	    SFINAE_ON_TEMPLATE_ARG(Is ## UNARY_TEX<D>)>	  /* Enable only for the UNARY_TEX required */ \
  DECLAUTO CALLER(D&& tex)      /*!< UnaryTEx to absorb         */	\
  {									\
    return forw<D>(tex);						\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Defines a simple way to swap nested UnaryTEx
#define UNARY_TEX_GOES_INSIDE(EXT_FUN,	 /*!< External builder */	\
			      UNARY_TEX, /*!< Name of the TEx  */	\
			      INT_FUN)	 /*!< Internal builder */	\
  /*! Simplify EXT_FUN(UNARY_TEX u) expression     */			\
  /*!                                              */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))              */			\
  template <typename D,                                   /* Type of the nested UNARY_TEX           */ \
	    SFINAE_ON_TEMPLATE_ARG(Is ## UNARY_TEX<D>)>	  /* Enable only for the UNARY_TEX required */ \
  DECLAUTO EXT_FUN(D&& tex)     /*!< UnaryTEx to nest           */	\
  {									\
    return INT_FUN(EXT_FUN(tex.ref));					\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
}

#endif
