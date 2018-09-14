#ifndef _UNARYSMET_HPP
#define _UNARYSMET_HPP

/// \file UnarySmET.hpp
///
/// \brief Header file defining basic properties of Unary SmET

#include <metaprogramming/SFINAE.hpp>
#include <smet/BaseSmET.hpp>
#include <smet/Reference.hpp>

namespace SUNphi
{
  /// Defines the UnarySmET type traits
  DEFINE_BASE_TYPE(UnarySmET,: public BaseSmET);
  
  // Defines the check for a member type "ref"
  DEFINE_HAS_MEMBER(ref);
  
  /// Defines the check for a Unary SmET
#define STATIC_ASSERT_IS_UNARY_SMET(...)			\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(ref,__VA_ARGS__)
  
  /// Unary SmET
  template <typename T>
  struct UnarySmET :
    public SmET<T>,
    public BaseUnarySmET
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Provide the reference to the object
#define PROVIDE_UNARY_SMET_REF			\
  PROVIDE_SMET_REF()
  
  /////////////////////////////////////////////////////////////////
  
  /// Set the assignability according to the reference
#define ASSIGNABLE_ACCORDING_TO_REF					\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can be assigned if the reference can */,RemoveReference<decltype(ref)>::isAssignable)
  
  /////////////////////////////////////////////////////////////////
  
  /// Set the mergeable components equal to the reference
#define MERGEABLE_ACCORDING_TO_REF					\
  PROVIDE_MERGEABLE_COMPS(/*! The components of this SmET can be merged as the reference one */,typename RemoveReference<decltype(ref)>::MergeableComps)
  
  /// Returns a version of the SmET with simple merging structure
#define PROVIDE_UNARY_SMET_SIMPLE_GET_MERGED_COMPS_VIEW(UNARY_SMET)	/* Name of the SmET type */ \
  PROVIDE_GET_MERGED_COMPS_VIEW(/*!  Returns a component-merged version */,	\
				auto refMerged=ref.template mergedComps<Is>(); \
				return UNARY_SMET<decltype(refMerged)>(std::move(refMerged)))
  
  /////////////////////////////////////////////////////////////////
  
  /// Set the size of components according to the reference
#define SAME_COMP_SIZES_OF_REF						\
  /*! Returns the size of a component, matching the reference */	\
  template <typename TC,						\
  	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>		\
  int compSize(SFINAE_WORSEN_DEFAULT_VERSION_ARGS) const		\
  {									\
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;				\
    return ref.template compSize<TC>();					\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Set the size of a specific component
#define PROVIDE_COMP_SIZES_FOR(TC,...)					\
  /*! Returns the size of component TC */				\
  template <typename T,							\
	    SFINAE_ON_TEMPLATE_ARG(isSame<T,TC>)>			\
  int compSize() const							\
  {									\
    __VA_ARGS__;							\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking a reference
#define PROVIDE_UNARY_SMET_SIMPLE_CREATOR(UNARY_SMET /*!< Name of the UnarySmET */) \
  /*! Constructor taking universal reference */				\
  template <typename SMET,						\
	    typename=EnableIf<isSame<Unqualified<SMET>,Unqualified<Ref>>>> \
  explicit UNARY_SMET(SMET&& smet) : ref(forw<SMET>(smet))		\
  {									\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Defines the assignement operator, calling assign
#define PROVIDE_UNARY_SMET_ASSIGNEMENT_OPERATOR(UNARY_SMET /*!< Name of the UnarySmET */) \
  /*! Assign from another smet */					\
  template <typename Oth>             	/* Other type  */		\
  UNARY_SMET& operator=(Oth&& smet)	/*!< Other SmET */		\
  {									\
  if(0)									\
    {									\
      using namespace std;						\
      cout<<"Operator=, triggering assignement to "<<this<<" of "<<&smet<<endl; \
    }									\
    assign(*this,forw<Oth>(smet));					\
									\
    return *this;							\
  }									\
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
  
  /// Create a simple builder with a name and a UNARY_SMET returned type
#define SIMPLE_UNARY_SMET_BUILDER(BUILDER,    /*!< Name of builder fun            */ \
				  UNARY_SMET) /*!< Name of the UnarySmET to build */ \
  /*! Simple UNARY_SMET builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain UNARY_SMET getting a plain SmET    */			\
  template <typename T, 	    /* Type of the SmET to get       */	\
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>		\
  UNARY_SMET<T> BUILDER(T&& smet,     /*!< SmET to act upon          */ \
			SFINAE_WORSEN_DEFAULT_VERSION_ARGS)		\
  {									\
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;				\
  									\
    if(0)								\
      {									\
	using namespace std;						\
	constexpr bool IsLvalue=isLvalue<T>;				\
	constexpr bool IsConst=isConst<T>;				\
	cout<<"Building " #UNARY_SMET " through " #BUILDER <<endl;	\
	constexpr bool Is=std::is_reference<T>::value;			\
	cout<<" IsLvalue: "<<IsLvalue<<", Is: "<<Is<<", IsConst: "<<IsConst<<endl; \
      }									\
    return UNARY_SMET<T>(forw<T>(smet));					\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Provides const or non-const the evaluator
#define PROVIDE_UNARY_SMET_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER) /*!< Const or not */ \
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
#define PROVIDE_UNARY_SMET_DEFAULT_EVALUATOR				\
  PROVIDE_UNARY_SMET_CONST_OR_NOT_DEFAULT_EVALUATOR(NON_CONST_QUALIF); \
  PROVIDE_UNARY_SMET_CONST_OR_NOT_DEFAULT_EVALUATOR(CONST_QUALIF)
  
  /// Implements a duplicated-call canceller
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// conj(conj(cicc)); // returns cicc
  /// \endcode
#define CANCEL_DUPLICATED_UNARY_SMET_CALL(CALLER,     /*!< Name of builder */ \
					  UNARY_SMET) /*!< Type to un-nest */ \
  /*! Simplify CALLER(UNARY_SMET) expression */				\
  /*!                                        */				\
  /*! Returns the nested reference           */				\
  template <typename T,                                        /* Type of the expression                  */ \
	    typename RrT=RemoveReference<T>,                   /* T without ref attributes                */ \
	    typename Ref=typename RrT::Ref,                    /* Type of the reference                   */ \
	    typename RrRef=RemoveReference<Ref>,               /* Ref without ref attributes              */ \
	    bool SmETIsLvalue=isLvalue<RrT>,		       /* Detect if SmET is an lvalue             */ \
	    bool RefIsLvalue=isLvalue<RrRef>,		       /* Detect if Ref is an lvalue              */ \
	    bool RefIsStoring=RrRef::isStoring,		       /* Detect if Ref is storing                */ \
	    bool RetByRef=RefIsStoring or	               /* Returns by val if Ref is storing, or    */ \
	    RefIsLvalue or SmETIsLvalue,	               /*   lvalue is involved         	          */ \
	    typename Ret=Conditional<RetByRef,RrRef&,RrRef>,   /* Returned type                           */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## UNARY_SMET<RrT>)>     /* Enable only for the UNARY_SMET required */ \
  Ret CALLER(T&& smet)	/*!< Quantity to un-nest   */			\
  {					\
    if(0)								\
      {									\
	constexpr bool SmETIsConst=isConst<T>;				\
	constexpr bool RefIsConst=isConst<Ref>;				\
									\
	using namespace std;						\
	cout<<"Removing duplicated call " # CALLER<<" "<<__PRETTY_FUNCTION__<<endl; \
	constexpr bool SmETIs=std::is_reference<T>::value;		\
	constexpr bool RefIs=std::is_reference<Ref>::value;		\
	cout<<" SmETIsLvalue: "<<SmETIsLvalue<<endl;			\
	cout<<" SmETIs: "<<SmETIs<<endl;					\
	cout<<" SmETIsConst: "<<SmETIsConst<<endl;			\
	cout<<" RefIsLvalue: "<<RefIsLvalue<<endl;			\
	cout<<" RefIs: "<<RefIs<<endl;					\
	cout<<" RefIsConst: "<<RefIsConst<<endl;			\
      }									\
									\
    return static_cast<Ret>(smet.ref);					\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Implements a duplicated-call absorber
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// wrap(wrap(cicc)); // returns wrap(cicc)
  /// \endcode
#define ABSORB_DUPLICATED_UNARY_SMET_CALL(CALLER,     /*!< Name of builder                */ \
					 UNARY_SMET)  /*!< Type to absorb                 */ \
  /*! Simplify CALLER(UNARY_SMET) expression */				\
  /*!                                       */				\
  /*! Returns the reference                 */				\
  template <typename D,                                   /* Type of the nested UNARY_SMET           */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## UNARY_SMET<D>)>	  /* Enable only for the UNARY_SMET required */ \
  DECLAUTO CALLER(D&& smet)      /*!< UnarySmET to absorb         */	\
  {									\
    return forw<D>(smet);						\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Defines a simple way to swap nested UnarySmET
  ///
  /// \todo we need to implement the same check done for
  /// CANCEL_DUPLICATED_UNARY_SMET_CALL
#define UNARY_SMET_GOES_INSIDE(EXT_FUN,	 /*!< External builder */	\
			      UNARY_SMET, /*!< Name of the SmET  */	\
			      INT_FUN)	 /*!< Internal builder */	\
  /*! Simplify EXT_FUN(UNARY_SMET u) expression     */			\
  /*!                                              */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))              */			\
  template <typename D,                                   /* Type of the nested UNARY_SMET           */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## UNARY_SMET<D>)>	  /* Enable only for the UNARY_SMET required */ \
  DECLAUTO EXT_FUN(D&& smet)     /*!< UnarySmET to nest           */	\
  {									\
    return INT_FUN(EXT_FUN(smet.ref));					\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Defines a simple way to swap an UnarySmET from rhs to lhs
  ///
  /// \todo why can't we make only const & on rhs?
  /// \todo we need to enforce SmET
#define UNARY_SMET_GOES_ON_LHS(LHS_FUN,	 /*!< External builder */	\
			      UNARY_SMET) /*!< Name of the SmET  */	\
  /*! Simplify EXT_FUN(UNARY_SMET u) expression     */			\
  /*!                                              */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))              */			\
  template <typename Lhs,                                   /* Type of the lhs SmET                    */ \
	    typename Rhs,                                   /* Type of the rhs UNARY_SMET              */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## UNARY_SMET<Rhs>)>   /* Enable only for the UNARY_SMET required */ \
  void assign(Lhs&& lhs,   /*!< Lhs of the assignement                         */ \
	      Rhs&& rhs)   /*!< Rhs of the assignement, to free from UNARY_SMET */ \
  {									\
    if(1)								\
      {									\
	using namespace std;						\
	cout<<"Moving " #UNARY_SMET "to lhs"<<endl;	\
      }									\
    assign(LHS_FUN(forw<Lhs>(lhs)),rhs.ref);				\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
}

#endif
