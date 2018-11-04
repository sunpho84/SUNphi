#ifndef _NNARYSMET_HPP
#define _NNARYSMET_HPP

/// \file NnarySmET.hpp
///
/// \brief Header file defining basic properties of Nnary SmET

#include <ints/IntSeqCat.hpp>
#include <smet/BaseSmET.hpp>
#include <tens/TensKind.hpp>
#include <tuple/TupleOrder.hpp>
#include <utility/Position.hpp>

namespace SUNphi
{
  /// Defines the NnarySmET type traits
  DEFINE_BASE_TYPE(NnarySmET,: public BaseSmET);
  
  // Defines the check for a member type "refs"
  DEFINE_HAS_MEMBER(refs);
  
  /// Defines the check for a Nnary SmET
#define STATIC_ASSERT_IS_NNARY_SMET(...)		\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(refs,__VA_ARGS__)
  
  /// Nnary SmET
  template <typename T>  /// Inheriting type
  struct NnarySmET :
    public SmET<T>,
    public BaseNnarySmET
  {
    IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can never be lhs */,false);
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Provide the references to the objects
  ///
  /// The reference types are contained in a \c Tuple with types
  /// obtained from template pars
#define PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(N)	\
  /*! Type of the binding references */			\
  using Refs=						\
    Tuple<_Refs...>;					\
  							\
  /*! Reference to the N object */			\
  Tuple<Reference<_Refs>...> refs;			\
  							\
  /*! Type of I-th \c SmET      */			\
  template <int I>					\
  using Ref=						\
    TupleElementType<I,Tuple<_Refs...>>;		\
							\
  /*! Number or \c SmET to act upon */			\
  static constexpr					\
  int NSmET=						\
    N;							\
							\
  /* Check that NSmET coincides with the par */		\
  static_assert(N==sizeof...(_Refs))
  
  /// Returns the size of a \c TensComp, with a simple approach
  ///
  /// Search the asked \c TensComp in each ref \c TensKind iteratively
  /// and returns the size in the first found ref
#define PROVIDE_SIMPLE_NNARY_COMP_SIZE					\
  /*! Returns the size in the first found ref */			\
  template <typename TC, /* \c TensComp to search           */		\
	    int I=0>     /* Integer of the ref to search in */		\
  int compSize() const							\
  {									\
    static_assert(I>=0 and I<NSmET,"Cannot search in negative or larger-than-N ref"); \
									\
    /*! \c TensKind of the I-th \c SmET */				\
    using RefTk=							\
      typename Ref<I>::Tk;						\
									\
    /*! Check if the \c TensKind contains the \c TensComp asked */	\
    constexpr bool found=						\
      RefTk::template contains<TC>;					\
									\
    /* Returns the size if it's in the I-th Tk */			\
    if constexpr(found)							\
      return get<I>(refs).template compSize<TC>();		        \
    else								\
      return compSize<TC,I+1>();					\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Proivde the position of result Tk \c TensComp in each input
#define PROVIDE_POS_OF_RES_TCS_IN_REFS					\
  /*! Position of all the Result \c TensComp in each \c Refs Tk */	\
  using PosOfResTcsInRefsTk=						\
    PosOfTcsOfTkInListOfTks<Tk,typename RemRef<_Refs>::Tk...>
  
  /// Provide the merging delimiters for each Ref, according to a request of merge
  ///
  /// Takes the position of the resulting types into each Ref, then it
  /// takes the delimiters position in such elelent list.
#define PROVIDE_MERGING_DELIMS_FOR_REFS					\
  /*! Merging delimiters for all Refs according to given MD */		\
  template <typename MD>     /* Required merging delimiters */		\
  using MergingDelimsForRefs=						\
    Tuple<InsertInOrderedIntSeq /* Insert the begin */			\
	  <0,        /* Insert 0 as begin */				\
	   IntSeqGetElsAfterAppending					\
	   <RemRef<_Refs>::Tk::nTypes,					\
	    false,   /* Omit NOT_PRESENT */				\
	    MD,	  							\
	    PosOfTypesNotAsserting<typename Tk::types,			\
				   typename RemRef<_Refs>::Tk::types>>, \
	   0,        /* Shift after inserting */			\
	   true>...> /* Ignore 0 if present   */
  
  /// Determine the mergeability of the \c ResPos component of the result
  ///
  /// Nested internal implemention
  /// \todo still not accepting external additional constraints
  template <int ResPos=0,                    // Current \c TensComp to be checked
	    typename...MergeDelim,           // \c IntSeq containing the allowed mergeability delimiters
	    typename...PosOfResTcsInRefTk,   // Route to give the position of each \c TensComp of the Res in each Ref
	    int...PrevPosInts>               // Position of previous component
  constexpr DECLAUTO _compsMergeability(Tuple<MergeDelim...>,
					Tuple<PosOfResTcsInRefTk...>,
					IntSeq<PrevPosInts...>)
  {
    /// Number of remaining \c TensComp to be checked
    constexpr int nC=
      TupleElementType<0,Tuple<PosOfResTcsInRefTk...>>::size;
    
    /// Check that all Pos contains the same number of \c TensComp
    static_assert(((nC==PosOfResTcsInRefTk::size) && ...),"All PosOfResTcsInRefTk must have the same size");
    
    // If we are terminating, return the final position
    if constexpr(nC==0)
      return IntSeq<ResPos>{};
    else
      {
	/// Slice with the poisition of the current component
	using CurPos=
	  IntSeq<PosOfResTcsInRefTk::first...>;
	
	/// Nested delimiters
	using Nested=
	  decltype(_compsMergeability<ResPos+1>(Tuple<MergeDelim...>{},
				      Tuple<IntSeqGetAllButFirst<PosOfResTcsInRefTk>...>{},
				      CurPos{}));
	
	/// Check whether the current component is not consecutive in all Refs
	constexpr bool curNotConsecutive=
	  ((PrevPosInts!=NOT_PRESENT and
	    PosOfResTcsInRefTk::first!=PrevPosInts+1) || ...);
	
	/// Check whether the current component was not originally mergeable
	constexpr bool originallyNotMergeable=
	  (MergeDelim::template has<PosOfResTcsInRefTk::first> || ...);
	
	/// Check where the current component is present
	using CurPres=
	  IntSeq<(PosOfResTcsInRefTk::first!=NOT_PRESENT)...>;
	
	/// Check where the previous component was present
	using PrevPres=
	  IntSeq<(PrevPosInts!=NOT_PRESENT)...>;
	
	/// Determine if a break in mergeability is needed
	constexpr bool insBreak=
	  originallyNotMergeable
	 or
	  (not isSame<PrevPres,CurPres>)
	 or
	  curNotConsecutive;
	
	/// Insert a break or not
	if constexpr(insBreak)
          return IntSeqCat<IntSeq<ResPos>,Nested>{};
	else
	  return Nested{};
      }
  }
  
  /// Determine the mergeability of a given \c TensComp
  ///
  /// A component is declared mergeable if its presence in all \c
  /// TensKind is of the same nature of the previous one (e.g. present
  /// only in one of the \c TensKind), if its position inside the \c
  /// TensKind is consecutive with previous \c TensComp, and if the
  /// component was mergeable in all \c TensKind
  template <typename ReqMD,                   // \c Tuple of \c IntSeq containing the required Merge Delim
	    typename PosOfResTcsInRefTks>     // Position of the result comps in the refs (\c Tuple of \c IntSeq)
  using CompsMergeability=
    decltype(_compsMergeability(ReqMD{},PosOfResTcsInRefTks{},IntSeqOfSameNumb<tupleSize<ReqMD>,0>{}));
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking n references
#define PROVIDE_NNARY_SMET_SIMPLE_CREATOR(NNARY_SMET /*!< Name of the NnarySmET */) \
  /*! Constructor taking universal reference */				\
  template <typename...SMETS,						\
	    typename=EnableIf<((isSame<Unqualified<SMETS>,Unqualified<_Refs>>) && ...)>> \
  explicit NNARY_SMET(SMETS&&...smets) : refs(forw<SMETS>(smets)...)	\
  {									\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
  /// Create a simple builder with a name and a NNARY_SMET returned type
  ///
  /// \c NnarySmET cannot use SFINAE to worsen default constructors
  /// (unless packing args in \c Tuple) so the simple builder cannot
  /// be overloaded. If required, use a custom builder and use if
  /// constexpr construct inside it to route the manipulations.
#define SIMPLE_NNARY_SMET_BUILDER(BUILDER,     /*!< Name of builder function        */ \
				  NNARY_SMET)  /*!< Name of the NnarySmET to build */ \
  /*! Simple NNARY_SMET builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain NNARY_SMET getting n plain SmET    */			\
  template <typename...Ts> 	   /* Types of the SmET to get       */ \
  NNARY_SMET<Ts...> BUILDER(Ts&&...smets)    /*!< SmETs to act upon  */ \
  {									\
    return NNARY_SMET<Ts...>(forw<Ts>(smets)...);			\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Set aliasing according to the isAliasing of references
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_REFS			\
  /*! Forward aliasing check to the references */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    CRASH("Fixme");					\
    return true;					\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
}

#endif
