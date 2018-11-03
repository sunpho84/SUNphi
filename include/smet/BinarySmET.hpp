#ifndef _BINARYSMET_HPP
#define _BINARYSMET_HPP

/// \file BinarySmET.hpp
///
/// \brief Header file defining basic properties of Binary SmET

#include <ints/IntSeqCat.hpp>
#include <smet/BaseSmET.hpp>
#include <utility/Position.hpp>

namespace SUNphi
{
  /// Defines the BinarySmET type traits
  DEFINE_BASE_TYPE(BinarySmET,: public BaseSmET);
  
  // Defines the check for a member type "ref1"
  DEFINE_HAS_MEMBER(ref1);
  
  // Defines the check for a member type "ref2"
  DEFINE_HAS_MEMBER(ref2);
  
  /// Defines the check for a Binary SmET
#define STATIC_ASSERT_IS_BINARY_SMET(...)		\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(ref1,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(ref2,__VA_ARGS__)
  
  /// Binary SmET
  template <typename T>
  struct BinarySmET :
    public SmET<T>,
    public BaseBinarySmET
  {
    IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can never be lhs */,false);
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Provide the reference to the objects
#define PROVIDE_BINARY_SMET_REFS			\
  PROVIDE_SMET_REF(1);					\
  PROVIDE_SMET_REF(2)
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking a reference
#define PROVIDE_BINARY_SMET_SIMPLE_CREATOR(BINARY_SMET /*!< Name of the BinarySmET */) \
  /*! Constructor taking universal reference */				\
  template <typename SMET1,						\
	    typename SMET2,						\
	    typename=EnableIf<isSame<Unqualified<SMET1>,Unqualified<Ref1>>>, \
	    typename=EnableIf<isSame<Unqualified<SMET2>,Unqualified<Ref2>>>> \
  explicit BINARY_SMET(SMET1&& smet1,SMET2&& smet2) : ref1(forw<SMET1>(smet1)),ref2(forw<SMET2>(smet2)) \
  {									\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
  /// Create a simple builder with a name and a BINARY_SMET returned type
#define SIMPLE_BINARY_SMET_BUILDER(BUILDER,     /*!< Name of builder function        */ \
				   BINARY_SMET) /*!< Name of the BinarySmET to build */ \
  /*! Simple BINARY_SMET builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain BINARY_SMET getting a pair of plain SmET                 */ \
  template <typename T1, 	   /* Type of the first SmET to get  */ \
	    typename T2, 	   /* Type of the second SmET to get */ \
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>		\
  BINARY_SMET<T1,T2> BUILDER(T1&& smet1,   /*!< First SmET to act upon  */ \
			     T2&& smet2,   /*!< Second SmET to act upon */ \
			     SFINAE_WORSEN_DEFAULT_VERSION_ARGS)	\
  {									\
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;				\
    									\
    return BINARY_SMET<T1,T2>(forw<T1>(smet1),forw<T2>(smet2));		\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Set aliasing according to the isAliasing of references 1 and 2
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_PAIR_OF_REFS			\
  /*! Forward aliasing check to the references */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    return						\
      ref1.isAliasing(alias) or				\
      ref2.isAliasing(alias);				\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Determine mergeability of pair of \c TensKind
  namespace PairOfTensKindMergeability
  {
    using AbsentInBoth=
      IntSeq<false,false>;
    
    /// Determine the mergeability of a given \c TensComp
    ///
    /// Terminator of the nested implementation, returning an \c
    /// IntSeq holding just the current searched position, as a final
    /// delimiter
    template <typename PrevPres=AbsentInBoth,   // Previous position presence
	      int ResPos=0,                     // Position in the result probed
	      int PrevPos1=0,                   // Previous position in the first \c TensKind
	      int PrevPos2=0,                   // Previous position in the second \c TensKind
	      int...MDel1,                      // Mergeability delimiters of first \c SmET
	      int...MDel2>                      // Mergeability delimiters of second \c SmET
    DECLAUTO _compsMergeability(IntSeq<MDel1...> MD1, ///< Holds the mergeability delimiters of first \c SmET
				IntSeq<MDel2...> MD2, ///< Holds the mergeability delimiters of second \c SmET
				IntSeq<> P1,          ///< Holds the position of first \c SmET where the \c Res \c TensComp are found
				IntSeq<> P2)          ///< Holds the position of second \c SmET where the \c Res \c TensComp are found
    {
      return IntSeq<ResPos>{};
    }
    
    /// Determine the mergeability of a given \c TensComp, provided two \c TensKind
    ///
    /// Nested internal implementation, catting the current component
    /// with the tail mergeability.
    template <typename PrevPres=AbsentInBoth,   // Previous position presence
	      int ResPos=0,                     // Position probed
	      int PrevPos1=0,                   // Previous position in the first \c TensKind
	      int PrevPos2=0,                   // Previous position in the second \c TensKind
	      int...MDel1,                      // Mergeability delimiters of first \c SmET
	      int...MDel2,                      // Mergeability delimiters of second \c SmET
	      int Head1,                        // Current component position in the first \c SmET
	      int...Tail1,                      // Other components position in the first \c SmET
	      int Head2,                        // Current component position in the second \c SmET
	      int...Tail2>                      // Other components position in the second \c SmET
    DECLAUTO _compsMergeability(IntSeq<MDel1...> MD1,         ///< Holds the mergeability delimiters of first \c SmET
				IntSeq<MDel2...> MD2,         ///< Holds the mergeability delimiters of second \c SmET
				IntSeq<Head1,Tail1...> P1,    ///< Holds the position of first \c SmET where the \c TensComp is found
				IntSeq<Head2,Tail2...> P2)    ///< Holds the position of second \c SmET where the \c TensComp is found
    {
      /// Presence of current position
      using CurPres=
	IntSeq<Head1!=NOT_PRESENT,Head2!=NOT_PRESENT>;
      static_assert(not isSame<CurPres,AbsentInBoth>,"Cannot be AbsentInBoth");
      
      /// Check consecutivity in first \c TensKind
      constexpr int curNotConsecutive1=
	(PrevPos1!=NOT_PRESENT)
	and
	(Head1!=PrevPos1+1);
      
      /// Check consecutivity in second \c TensKind
      constexpr int curNotConsecutive2=
	(PrevPos2!=NOT_PRESENT)
	and
	(Head2!=PrevPos2+1);
      
      /// Result of mergeability of nested components
      using Nested=
	decltype(_compsMergeability<CurPres,ResPos+1,Head1,Head2>(IntSeq<MDel1...>{},IntSeq<MDel2...>{},IntSeq<Tail1...>{},IntSeq<Tail2...>{}));
      
      /// Check if the component was mergeable in the first \c TensKind
      constexpr bool originallyNotMergeableIn1=
	((MDel1==Head1) || ...);
      
      /// Check if the component was mergeable in the second \c TensKind
      constexpr bool originallyNotMergeableIn2=
	((MDel2==Head2) || ...);
      
      /// Determine if a break in mergeability is needed
      constexpr bool insBreak=
	originallyNotMergeableIn1
      or
	originallyNotMergeableIn2
      or
	(not isSame<PrevPres,CurPres>)
      or
	curNotConsecutive1
      or
	curNotConsecutive2;
      
      /// Insert a break or not
      if constexpr(insBreak)
        return IntSeqCat<IntSeq<ResPos>,Nested>{};
      else
	return Nested{};
    }
    
    /// Determine the mergeability of a given \c TensComp
    ///
    /// A component is declared mergeable if its presence in the two
    /// \c TensKind is of the same nature of the previous one
    /// (e.g. present only in one of the two \c TensKind, if its
    /// position inside the two \c TensKind is consecutive with
    /// previous \c TensComp, and if the component was mergeable in
    /// both \c TensKind
    template <typename MD1,
	      typename MD2,
	      typename I1,
	      typename I2>
    using CompsMergeability=
      decltype(_compsMergeability(MD1{},MD2{},I1{},I2{}));
  
  /// Provide component-mergeabilty according to the two refs, and the visible \c Tk
  ///
  /// \todo check that this is general enough... probably not
#define PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_TWO_REFS			\
  PROVIDE_MERGEABLE_COMPS(/*! Defer the mergeability to the internal implementation */, \
			  PairOfTensKindMergeability::template CompsMergeability< \
			  typename RemRef<Ref1>::MergeableComps,	\
			  typename RemRef<Ref2>::MergeableComps,	\
			  PosOfRef1TcsInResTk,				\
			  PosOfRef2TcsInResTk>)
  }
}

#endif
