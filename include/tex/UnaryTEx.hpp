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
#define PROVIDE_UNARY_TEX_REF			\
  PROVIDE_TEX_REF()
  
  /////////////////////////////////////////////////////////////////
  
  /// Set the assignability according to the reference
#define ASSIGNABLE_ACCORDING_TO_REF					\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx can be assigned according to the reference */,RemoveReference<decltype(ref)>::isAssignable)
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking a reference
#define PROVIDE_UNARY_TEX_SIMPLE_CREATOR(UNARY_TEX /*!< Name of the UnaryTEx */) \
  /*! Constructor taking universal reference */				\
  template <typename TEX,						\
	    typename=EnableIf<IsSame<Unqualified<TEX>,Unqualified<Ref>>>> \
  explicit UNARY_TEX(TEX&& tex) : ref(forw<TEX>(tex))			\
  {									\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Defines the assignement operator, calling assign
#define PROVIDE_UNARY_TEX_ASSIGNEMENT_OPERATOR(UNARY_TEX /*!< Name of the UnaryTEx */) \
  /*! Assign from another tex */					\
  template <typename Oth>             	/* Other type  */		\
  UNARY_TEX& operator=(Oth&& tex)	/*!< Other TEx */		\
  {									\
  if(0)									\
    {									\
      using namespace std;						\
      cout<<"Operator=, triggering assignement to "<<this<<" of "<<&tex<<endl; \
    }									\
    assign(*this,forw<Oth>(tex));					\
									\
    return *this;							\
  }									\
  /*il binder sta venendo distrutto anzitempo*/	\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
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
				 UNARY_TEX) /*!< Name of the UnaryTEx to build */ \
  /*! Simple UNARY_TEX builder called BUILDER */			\
  /*!                                         */			\
  /*! Plain UNARY_TEX getting a plain TEx     */			\
  template <typename T, 	    /* Type of the TEx to get       */	\
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>		\
  UNARY_TEX<T> BUILDER(T&& tex,     /*!< TEx to act upon            */	\
		       SFINAE_WORSEN_DEFAULT_VERSION_ARGS)		\
  {									\
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;				\
  									\
    if(0)								\
      {									\
	using namespace std;						\
	constexpr bool IsLvalue=isLvalue<T>;				\
	constexpr bool IsConst=isConst<T>;				\
	cout<<"Building " #UNARY_TEX " through " #BUILDER <<endl;	\
	constexpr bool Is=std::is_reference<T>::value;			\
	cout<<" IsLvalue: "<<IsLvalue<<", Is: "<<Is<<", IsConst: "<<IsConst<<endl; \
      }									\
    return UNARY_TEX<T>(forw<T>(tex));					\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Provides const or non-const the evaluator
#define PROVIDE_UNARY_TEX_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER) /*!< Const or not */ \
  /*! QUALIFIER Evaluator                                               */ \
  /*! \todo add/check that const qualifier is properly added to output  */ \
  template <typename...Args>						\
  DECLAUTO eval(const Args&...args) /*!< Parameters to pass */		\
    QUALIFIER								\
  {									\
    STATIC_ASSERT_ARE_N_TYPES(TK::nTypes,args);				\
    return ref.eval(forw<const Args>(args)...);				\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Provides both const and non-const evaluators
#define PROVIDE_UNARY_TEX_DEFAULT_EVALUATOR				\
  PROVIDE_UNARY_TEX_CONST_OR_NOT_DEFAULT_EVALUATOR(NON_CONST_QUALIF); \
  PROVIDE_UNARY_TEX_CONST_OR_NOT_DEFAULT_EVALUATOR(CONST_QUALIF)
  
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
  template <typename T,                                    /* Type of the expression                 */ \
	    typename Ref=typename RemoveReference<T>::Ref, /* Type of the reference                  */ \
	    bool TexIsLvalue=isLvalue<T>,		   /* Detect if TEx is an lvalue             */ \
	    bool RefIsLvalue=isLvalue<Ref>,		   /* Detect if Ref is an lvalue             */ \
	    bool RefIsStoring=Ref::isStoring,		   /* Detect if Ref is storing               */ \
	    bool Move=RefIsStoring and			   /* Move only if Ref is storing, and 	     */	\
	    not (RefIsLvalue or TexIsLvalue),		   /*   no lvalue is involved          	     */	\
	    typename Ret=Conditional<Move,Ref&&,Ref>,      /* Returned type                          */ \
	    SFINAE_ON_TEMPLATE_ARG(Is ## UNARY_TEX<T>)>	   /* Enable only for the UNARY_TEX required */ \
  Ret CALLER(T&& tex)	/*!< Quantity to un-nest   */			\
  {									\
    if(0)								\
      {									\
	constexpr bool TexIsConst=isConst<T>;				\
	constexpr bool RefIsConst=isConst<Ref>;				\
									\
	using namespace std;						\
	cout<<"Removing duplicated call " # CALLER<<" "<<__PRETTY_FUNCTION__<<endl; \
	constexpr bool TexIs=std::is_reference<T>::value;		\
	constexpr bool RefIs=std::is_reference<Ref>::value;		\
	cout<<" TexIsLvalue: "<<TexIsLvalue<<endl;			\
	cout<<" TexIs: "<<TexIs<<endl;					\
	cout<<" TexIsConst: "<<TexIsConst<<endl;			\
	cout<<" RefIsLvalue: "<<RefIsLvalue<<endl;			\
	cout<<" RefIs: "<<RefIs<<endl;					\
	cout<<" RefIsConst: "<<RefIsConst<<endl;			\
      }									\
									\
    return static_cast<Ret>(tex.ref);					\
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
  template <typename D,                                   /* Type of the nested UNARY_TEX           */ \
	    SFINAE_ON_TEMPLATE_ARG(Is ## UNARY_TEX<D>)>	  /* Enable only for the UNARY_TEX required */ \
  DECLAUTO CALLER(D&& tex)      /*!< UnaryTEx to absorb         */	\
  {									\
    return forw<D>(tex);						\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Defines a simple way to swap nested UnaryTEx
  ///
  /// \todo we need to implement the same check done for
  /// CANCEL_DUPLICATED_UNARY_TEX_CALL
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
  
  /// Defines a simple way to swap an UnaryTEx from rhs to lhs
  ///
  /// \todo why can't we make only const & on rhs?
  /// \todo we need to enforce TEx
#define UNARY_TEX_GOES_ON_LHS(LHS_FUN,	 /*!< External builder */	\
			      UNARY_TEX) /*!< Name of the TEx  */	\
  /*! Simplify EXT_FUN(UNARY_TEX u) expression     */			\
  /*!                                              */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))              */			\
  template <typename Lhs,                                   /* Type of the lhs TEx                    */ \
	    typename Rhs,                                   /* Type of the rhs UNARY_TEX              */ \
	    SFINAE_ON_TEMPLATE_ARG(Is ## UNARY_TEX<Rhs>)>   /* Enable only for the UNARY_TEX required */ \
  void assign(Lhs&& lhs,   /*!< Lhs of the assignement                         */ \
	      Rhs&& rhs)   /*!< Rhs of the assignement, to free from UNARY_TEX */ \
  {									\
    if(1)								\
      {									\
	using namespace std;						\
	cout<<"Moving UNARY_TEX to lhs "<<lhs.getStor()._v<<endl;	\
      }									\
    assign(LHS_FUN(forw<Lhs>(lhs)),rhs.ref);				\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
}

#endif
