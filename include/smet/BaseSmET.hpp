#ifndef _BASESMET_HPP
#define _BASESMET_HPP

/// \file BaseSmET.hpp
///
/// \brief Header file defining basic properties of Smart Expression Templates

#include <ints/IntSeqGetEl.hpp>
#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/SwallowSemicolon.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

#include <iostream>

namespace SUNphi
{
  /// Provides an attribute
#define STATIC_CONSTEXPR(DESCRIPTION,LONG_DESCRIPTION,TYPE,NAME,...)	\
  DESCRIPTION								\
  /*!             */							\
  LONG_DESCRIPTION							\
  static constexpr TYPE NAME=						\
    __VA_ARGS__
  
  /// Provides an attribute
#define USING(DESCRIPTION,LONG_DESCRIPTION,NAME,...)		\
  DESCRIPTION							\
  /*!             */						\
  LONG_DESCRIPTION						\
  using NAME=							\
    __VA_ARGS__
  
  /// Define a getter of an attibute, with a default
#define DEFINE_GETTER_WITH_DEFAULT(NAME,  /*!< Name of the attribute */ \
				   DEFVAL /*!< Default value         */) \
  /*! Provides a function to get NAME, or a DEFVAL if not present    */	\
  /*!  and no external \c Defval is provided                         */ \
  /*!                                                                */ \
  /*! Internal implentation                                          */ \
  template <typename T,							\
	    auto DefVal=DEFVAL>						\
  constexpr bool _get ## NAME() /*!< Type to analyze                 */ \
  {									\
    if constexpr(isClass<T>)						\
      if constexpr(hasMember_ ## NAME<T>)			        \
	return RemRef<T>::NAME;			                        \
									\
      return DefVal;							\
  }									\
									\
  /*! Provides a function to get NAME, or a DEFVAL if not present */	\
  /*!                                                             */	\
  /*! Gives visibility to internal implementation                 */	\
  template <typename T,							\
	    auto DefVal=DEFVAL>						\
  [[ maybe_unused ]]							\
  constexpr bool NAME=							\
    _get ## NAME<T,DEFVAL>()
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines the BaseSmET type traits
  DEFINE_BASE_TYPE(SmET);
  
  // Defines the check for a member "name"
  DEFINE_HAS_MEMBER(name);
  
  // Defines the check for a member "eval"
  DEFINE_HAS_MEMBER(eval);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member "isStoring"
  DEFINE_HAS_MEMBER(isStoring);
  
  /// Provides a isStoring attribute
#define IS_STORING_ATTRIBUTE(LONG_DESCRIPTION,...)			\
  STATIC_CONSTEXPR(/*! Returns whether this SmET is storing */,LONG_DESCRIPTION,bool,isStoring,__VA_ARGS__)
  
  /// Set the \c SmET to storing
#define STORING						\
  IS_STORING_ATTRIBUTE(/*! This \c SmET is storing */,true)
  
  /// Set the \c SmET to not-storing
#define NOT_STORING						\
  IS_STORING_ATTRIBUTE(/*! This \c SmET is not storing */,false)
  
  DEFINE_GETTER_WITH_DEFAULT(isStoring,false);
  
  /////////////////////////////////////////////////////////////////
  
  /// Provides the \c Tk member
#define PROVIDE_TK(...)					\
  using Tk=						\
    __VA_ARGS__
  
  // Defines the check for a member type \c Tk
  DEFINE_HAS_MEMBER(Tk);
  
  /// Returns the fundamental type of a class
  template <typename T,
	    typename=ConstrainIsSmET<T>>
  using TkOf=
    typename RemRef<T>::Tk;
  
  /// Provides the \c Fund member
#define PROVIDE_FUND(...)			\
  using Fund=					\
    __VA_ARGS__
  
  // Defines the check for a member type \c Fund
  DEFINE_HAS_MEMBER(Fund);
  
  /// Returns the fundamental type of a class
  template <typename T,
	    typename=ConstrainIsSmET<T>>
  using FundTypeOf=
    typename RemRef<T>::Fund;
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member variable \c isAliasing
  DEFINE_HAS_MEMBER(isAliasing);
  
  /// Provides a \c isAliasing method, taking \c alias as argument
#define PROVIDE_IS_ALIASING(LONG_DESCRIPTION,...)	\
  LONG_DESCRIPTION					\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    __VA_ARGS__;					\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Set aliasing according to a passed pointer (provided class member)
  ///
  /// \todo This is mostly broken
#define IS_ALIASING_ACCORDING_TO_POINTER(_p)				\
  PROVIDE_IS_ALIASING( /*! Check the aliasing with reference */,	\
		       CRASH<<"Broken";				\
									\
		       const void* pAlias=				\
		         static_cast<const void*>(&alias);		\
		       const void* p=					\
		         static_cast<const void*>(_p);			\
									\
		       return pAlias==p;)
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member variable \c isAssignable
  DEFINE_HAS_MEMBER(isAssignable);
  
  /// Provides a \c isAssignable attribute
#define IS_ASSIGNABLE_ATTRIBUTE(LONG_DESCRIPTION,...)			\
  STATIC_CONSTEXPR(/*! Returns whether this \c SmET can be the left hand of an assignement */,LONG_DESCRIPTION,bool,isAssignable,__VA_ARGS__)
  
  /// Set the \c SmET as assignable
#define ASSIGNABLE						\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This \c SmET can be assigned */,true)
  
  /// Set the \c SmET as not-assignable
#define NOT_ASSIGNABLE						\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This \c SmET cannot be assigned */,false)
  
  /////////////////////////////////////////////////////////////////
  
  /// Provide the MergeableComps type, \c assertMergebaleWith and \c getMaximallyMergedCompsView
  ///
  /// The type must be an ordered \c IntSeq indicating the splitting
  /// point of the \c TensKind, as in this example:
  ///
  /// \code
  /// // Maximal splitting
  /// Using Tk=
  ///   TensKind<Spin,Col>
  /// PROVIDE_MERGEABLE_COMPS(/*! Description */,IntSeq<0,2>);
  /// \endcode
#define PROVIDE_MERGEABLE_COMPS(LONG_DESCRIPTION,...)			\
  USING(/*! List of points where the \c TensKind can be split */,LONG_DESCRIPTION,MergeableComps,__VA_ARGS__); \
									\
  /*! Assert that a given merging condition is valid */			\
  template <typename Is,						\
	    typename=EnableIf<isIntSeq<Is>>>				\
  static constexpr void assertMergeableWith()				\
  {									\
    static_assert(Unqualified<MergeableComps>::template isSubsetOf<Is>,"Unable to merge this components"); \
  }									\
									\
  /*! Returns the maximal possible merged comps view */			\
  DECLAUTO getMaximallyMergedCompsView()				\
  {									\
    return getMergedCompsView<MergeableComps>();			\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// States that the class has no mergeable component
#define NOT_MERGEABLE				\
  PROVIDE_MERGEABLE_COMPS(/*! The class has no mergeable componentes */,IntSeq<>)
  
  /// States that the class has no mergeable component
#define PROVIDE_MERGEABLE_COMPS_MARKING_ONE_AS_NON_MERGEABLE(LONG_DESCRIPTION,IN_MERGEABLE_COMPS,POS) \
  PROVIDE_MERGEABLE_COMPS(LONG_DESCRIPTION,				\
			  InsertIntSeqInOrderedIntSeq<			\
			  IntSeq<POS,POS+1>,   /*! Position where to insert      */ \
			  IN_MERGEABLE_COMPS,  /*! Incoming mergeable components */ \
			  IntSeq<0,0>,         /*! Shift 0 after insertion       */ \
			  true>)               /*! Ignore if already present     */
  
  // Check that we know which components can be merged
  DEFINE_HAS_MEMBER(MergeableComps);
  
  // Check that we know how to check if merge is valid
  DEFINE_HAS_MEMBER(assertMergeableWith);
  
  /// Provides a \c getMergedCompsView method, taking Is as template parameter
#define PROVIDE_GET_MERGED_COMPS_VIEW(DESCRIPTION,...) \
  DESCRIPTION								\
  template <typename Is>       /* IntSeq delimiting the comps groups */ \
  DECLAUTO getMergedCompsView()						\
    const								\
  {									\
    /* Check that we can merge as asked */				\
    assertMergeableWith<Is>();						\
									\
    __VA_ARGS__;							\
  }									\
									\
  PROVIDE_ALSO_NON_CONST_METHOD(getMergedCompsView)
  
  // Check that we know how to get a MergedCompsView
  DEFINE_HAS_MEMBER(getMergedCompsView);
  
  /// Provides the \c MergedDelim attribute for Refs of a given \c ID
  ///
  /// \todo Add some more explenation...
#define PROVIDE_POS_OF_TCS_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(ID)	\
  /*! Position of all the \c TensComp of \c Ref ## ID in Res Tk */	\
  using PosOfRef ## ID ## TcsInResTk=					\
    PosOfTypesNotAsserting<typename Tk::types,typename TK ## ID::types>; \
  									\
  /*! Position of all the \c TensComp of \c Ref ## ID present in Res Tk */ \
  using PosOfRef ## ID ## PresTcsInResTk=				\
    FilterVariadicClassPos<IsPresent,PosOfRef ## ID ## TcsInResTk>; \
  									\
  /*! Merged delimiters of Ref ## ID according to MD */			\
  template <typename MD> /* Required merging delimiters */		\
  using MergedDelims ## ID=						\
    IntSeqGetElsAfterAppending<TK ## ID::nTypes,false,MD,PosOfRef ## ID ## TcsInResTk>
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines the check for a SmET
  ///
  /// \todo add costRead
  /// \todo add costEval
  /// \todo add isAliasing
#define STATIC_ASSERT_IS_SMET(...)				\
  STATIC_ASSERT_HAS_MEMBER(eval,__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(compSize,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isAliasing,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isAssignable,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isStoring,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(MergeableComps,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(assertMergeableWith,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(getMergedCompsView,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(Tk,__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(Fund,__VA_ARGS__)
  
  /// Smart Expression Templates
  template <typename T>
  struct SmET :
    public BaseSmET
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Provide the call operator with a given qualifier
#define PROVIDE_CALL_OPERATOR(QUALIFIER)				\
    /*! Implements the QUALIFIER call operator via \c CRTP          */	\
    template <typename...Oth>     /* Type of the other quantity     */	\
    DECLAUTO operator()(Oth&&...oth) QUALIFIER /*!< Other quantit   */	\
    {									\
      if constexpr(0)							\
	std::cout<<"Using CRTP to cast call operator"<<std::endl;       \
									\
      return (~(*this)).eval(forw<Oth>(oth)...);			\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CALL_OPERATOR();
    PROVIDE_CALL_OPERATOR(const);
    
#undef PROVIDE_CALL_OPERATOR
  };
  
  /// Defines a simple way to swap an SmET from rhs to lhs
  ///
  /// \todo why can't we make only const & on rhs?
  /// \todo we need to enforce SmET
#define SMET_GOES_ON_LHS(LHS_FUN,	   /*!< External builder     */	\
			 SMET)             /*!< Name of the \c SmET  */	\
  /*! Simplify EXT_FUN(      SMET u) expression    */			\
  /*!                                              */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))              */			\
  template <typename Lhs,                              /* Type of the lhs \c SmET              */ \
	    typename Rhs,                              /* Type of the rhs \c SMET              */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## SMET<Rhs>)>   /* Enable only for the \c SMET required */ \
  void assign(Lhs&& lhs,   /*!< Lhs of the assignement                       */ \
	      Rhs&& rhs)   /*!< Rhs of the assignement, to free from \c SMET */ \
  {									\
    if(1)								\
      {									\
	using namespace std;						\
	cout<<"Moving " #SMET "to lhs"<<endl;				\
      }									\
    assign(LHS_FUN(forw<Lhs>(lhs)),rhs.ref);				\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
}

#endif
