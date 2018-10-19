#ifndef _BASESMET_HPP
#define _BASESMET_HPP

/// \file BaseSmET.hpp
///
/// \brief Header file defining basic properties of Smart Expression Templates

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/SwallowSemicolon.hpp>

namespace SUNphi
{
  /// Provides an attribute
#define STATIC_CONSTEXPR(DESCRIPTION,LONG_DESCRIPTION,TYPE,NAME,...)	\
  DESCRIPTION								\
  /*!             */							\
  LONG_DESCRIPTION							\
  static constexpr TYPE NAME=__VA_ARGS__
  
  /// Provides an attribute
#define USING(DESCRIPTION,LONG_DESCRIPTION,NAME,...)		\
  DESCRIPTION							\
  /*!             */						\
  LONG_DESCRIPTION						\
  using NAME=__VA_ARGS__
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines the BaseSmET type traits
  DEFINE_BASE_TYPE(SmET);
  
  // Defines the check for a member "name"
  DEFINE_HAS_MEMBER(name);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member "isStoring"
  DEFINE_HAS_MEMBER(isStoring);
  
  /// Provides a isStoring attribute
#define IS_STORING_ATTRIBUTE(LONG_DESCRIPTION,...)			\
  STATIC_CONSTEXPR(/*! Returns whether this SmET is storing */,LONG_DESCRIPTION,bool,isStoring,__VA_ARGS__)
  
  /// Set the SmET to storing
#define STORING						\
  IS_STORING_ATTRIBUTE(/*! This SmET is storing */,true)
  
  /// Set the SmET to not-storing
#define NOT_STORING						\
  IS_STORING_ATTRIBUTE(/*! This SmET is not storing */,false)
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member type "Tk"
  DEFINE_HAS_MEMBER(Tk);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member variable "isAliasing"
  DEFINE_HAS_MEMBER(isAliasing);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member variable "isAssignable"
  DEFINE_HAS_MEMBER(isAssignable);
  
  /// Provides a isAssignable attribute
#define IS_ASSIGNABLE_ATTRIBUTE(LONG_DESCRIPTION,...)			\
  STATIC_CONSTEXPR(/*! Returns whether this SmET can be the left hand of an assignement */,LONG_DESCRIPTION,bool,isAssignable,__VA_ARGS__)
  
  /// Set the SmET as assignable
#define ASSIGNABLE						\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can be assigned */,true)
  
  /// Set the SmET as not-assignable
#define NOT_ASSIGNABLE						\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET cannot be assigned */,false)
  
  /////////////////////////////////////////////////////////////////
  
  /// Provide the MergeableComps type, assertMergebaleWith amd getMaximallyMergedCompsView
  ///
  /// The type must be an ordered IntSeq indicating the splitting
  /// point of the TensKind, as in this example:
  ///
  /// \code
  /// // Maximal splitting
  /// Using Tk=TensKind<Spin,Col>
  /// PROVIDE_MERGEABLE_COMPS(IntSeq<0,2>);
  /// \endcode
  /// Provides a isAssignable attribute
#define PROVIDE_MERGEABLE_COMPS(LONG_DESCRIPTION,...)			\
  USING(/*! List of points where the TensKind can be split */,LONG_DESCRIPTION,MergeableComps,__VA_ARGS__); \
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
			  IntSeq<POS,POS+1>,   /* Position where to insert      */ \
			  IN_MERGEABLE_COMPS,  /* Incoming mergeable components */ \
			  IntSeq<0,0>,         /* Shift 0 after insertion       */ \
			  true>)               /* Ignore if already present     */
  
  // Check that we know which components can be merged
  DEFINE_HAS_MEMBER(MergeableComps);
  
  // Check that we know how to check if merge is valid
  DEFINE_HAS_MEMBER(assertMergeableWith);
  
  /// Provides a \c getMergedCompsView method, taking Is as template parameter
#define PROVIDE_CONST_OR_NOT_GET_MERGED_COMPS_VIEW(QUALIFIER,DESCRIPTION,...) \
  DESCRIPTION								\
  template <typename Is>       /* IntSeq delimiting the comps groups */ \
  DECLAUTO getMergedCompsView()						\
    QUALIFIER								\
  {									\
    /* Check that we can merge as asked */				\
    assertMergeableWith<Is>();						\
									\
    __VA_ARGS__;							\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Provides a const and not const \c getMergedCompsView metod
#define PROVIDE_GET_MERGED_COMPS_VIEW(DESCRIPTION,...)			\
  PROVIDE_CONST_OR_NOT_GET_MERGED_COMPS_VIEW(,DESCRIPTION,__VA_ARGS__);	\
  PROVIDE_CONST_OR_NOT_GET_MERGED_COMPS_VIEW(const,DESCRIPTION,__VA_ARGS__)
  
  // Check that we know how to get a MergedCompsView
  DEFINE_HAS_MEMBER(getMergedCompsView);
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines the check for a SmET
  ///
  /// \todo add costRead
  /// \todo add costEval
  /// \todo add isAliasing
#define STATIC_ASSERT_IS_SMET(...)				\
  STATIC_ASSERT_HAS_MEMBER(compSize,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isAliasing,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isAssignable,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isStoring,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(MergeableComps,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(assertMergeableWith,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(getMergedCompsView,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(Tk,__VA_ARGS__)
  
  /// Smart Expression Templates
  template <typename T>
  struct SmET :
    public BaseSmET
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
}

#endif
