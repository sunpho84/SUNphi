#ifndef _ADD_HPP
#define _ADD_HPP

/// \file Add.hpp
///
/// \brief Defines a class which take the sum of two SmETs

#include <smet/Reference.hpp>
#include <smet/BinarySmET.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>

namespace SUNphi
{
  /// Determine mergeability of pair of \c TensKind according to a third one
  namespace PairOfTensKindMergeability
  {
    /// Possible outcome of where a component is found in a \c TensKind
    ///
    /// The component can be at the beginning, or it can be present in
    /// one or both \c TensKind
    enum Presence_t
      {
	BEGIN_OF_TK,         ///< Begin of the resulting \c TensKind
	ABSENT_IN_FIRST,     ///< The component is absent in the first \c TensKind
	ABSENT_IN_SECOND,    ///< The component is absent in the second \c TensKind
	PRESENT_IN_BOTH};    ///< The component is present in both \c TensKind
    
    /// Returns where the component is present in the two \c TensKind
    ///
    /// The two values \c A1 and \c A2 contains the position where the component is present
    ///
    /// If A1 is \c NOT_PRESENT, marks \c ABSENT_IN_FIRST
    /// If A2 is \c NOT_PRESENT, marks \c ABSENT_IN_SECOND
    /// Otherwise marks \c PRESENT_IN_BOTH
    template <int A1, // First component position
	      int A2> // Second component position
    [[ maybe_unused ]]
    constexpr Presence_t compPresenceInTKs=
      (A1==NOT_PRESENT)?ABSENT_IN_FIRST:
	       ((A2==NOT_PRESENT)?ABSENT_IN_SECOND:
		PRESENT_IN_BOTH);
    
    /// Determine the mergeability of a given \c TensComp
    ///
    /// Terminator of the nested implementation, returning an \c
    /// IntSeq holding just the current position, as final delimiter
    template <Presence_t PrevPres=BEGIN_OF_TK,  // Previous position presence
	      int ResPos=0,                     // Position probed
	      int PrevPos1=0,                   // Previous position in the first \c TensKind
	      int PrevPos2=0,                   // Previous position in the second \c TensKind
	      int...MPos1,                      // Mergeability delimiters of first SmET
	      int...MPos2>                      // Mergeability delimiters of second SmET
    DECLAUTO _compsMergeability(IntSeq<MPos1...> MP1, ///< Holds the mergeability delimiters of first SmET
				IntSeq<MPos2...> MP2, ///< Holds the mergeability delimiters of second SmET
				IntSeq<> P1,          ///< Holds the position of first SmET where the result TensComp are found
				IntSeq<> P2)          ///< Holds the position of second SmET where the result TensComp are found
    {
      return IntSeq<ResPos>{};
    }
    
    /// Determine the mergeability of a given \c TensComp
    ///
    /// Nested internal implementation, catting the current component
    /// with the tail mergeability.
    template <Presence_t PrevPres=BEGIN_OF_TK,  // Previous position presence
	      int ResPos=0,                     // Position probed
	      int PrevPos1=0,                   // Previous position in the first \c TensKind
	      int PrevPos2=0,                   // Previous position in the second \c TensKind
	      int...MPos1,                      // Mergeability delimiters of first SmET
	      int...MPos2,                      // Mergeability delimiters of second SmET
	      int Head1,                        // Current component position in the first SmET
	      int...Tail1,                      // Other components position in the first SmET
	      int Head2,                        // Current component position in the second SmET
	      int...Tail2>                      // Other components position in the second SmET
    DECLAUTO _compsMergeability(IntSeq<MPos1...> MP1,         ///< Holds the mergeability delimiters of first SmET
				IntSeq<MPos2...> MP2,         ///< Holds the mergeability delimiters of second SmET
				IntSeq<Head1,Tail1...> P1,    ///< Holds the position of first SmET where the result TensComp are found
				IntSeq<Head2,Tail2...> P2)    ///< Holds the position of second SmET where the result TensComp are found
    {
      /// Presence of current position
      constexpr Presence_t curPres=
	compPresenceInTKs<Head1,
			  Head2>;
      
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
	decltype(_compsMergeability<curPres,ResPos+1,Head1,Head2>(IntSeq<MPos1...>{},IntSeq<MPos2...>{},IntSeq<Tail1...>{},IntSeq<Tail2...>{}));
      
      /// Check if the component was mergeable in the first \c TensKind
      constexpr bool originallyNotMergeableIn1=
	((MPos1==Head1) || ...);
      
      /// Check if the component was mergeable in the second \c TensKind
      constexpr bool originallyNotMergeableIn2=
	((MPos2==Head2) || ...);
      
      /// Determine if a break in mergeability is needed
      constexpr bool insBreak=
	originallyNotMergeableIn1
      or
	originallyNotMergeableIn2
      or
	PrevPres!=curPres
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
    /// \c TensKind is of the same nature of the previous one, if its
    /// position inside the two \c TensKind is consecutive with
    /// previous \c TensComp, and if the component was mergeable
    template <typename MP1,
	      typename MP2,
	      typename I1,
	      typename I2>
    using CompsMergeability=
      decltype(_compsMergeability(MP1{},MP2{},I1{},I2{}));
    
    /////////////////////////////////////////////////////////////////
    
    /// Returns the position, if any, that the merge delimiter must assume in the reference
    template <int NRefComps,       // Number of components in the reference \c TensKind
	      int MDel,            // Res delimiter to be added
	      int...PosInRef>      // Position of the ResComp in the reference \c TensKind
    DECLAUTO mergedDelimInRef(IntSeq<PosInRef...>)
    {
      /// Positions assumed in the reference
      ///
      /// The number of component in Ref is added, for convenience, so
      /// to make it possible to search for the end delimiter
      using Is=
	IntSeq<PosInRef...,NRefComps>;
      
      /// Position of the component MDel
      constexpr int P=
	Is::template element<MDel>();
      
      // If the component is not present return empty
      if constexpr(P!=NOT_PRESENT)
	return IntSeq<P>{};
      else
	return IntSeq<>{};
    }
    
    /// Returns the positions, that the merge delimiter must assume in the reference
    ///
    /// Internal implementation, catting the result of the single delimiter search
    template <int NRefComps,       // Number of components in the reference \c TensKind
	      int...MDel,          // Res delimiters to be added
	      int...PosInRef>      // Position of the ResComp in the reference \c TensKind
    DECLAUTO mergedDelimsInRef(IntSeq<MDel...>,
			       IntSeq<PosInRef...>)
    {
      return IntSeqCat<decltype(mergedDelimInRef<NRefComps,MDel>(IntSeq<PosInRef...>{}))...>{};
    }
    
    /// Returns the positions, that the merge delimiter must assume in the reference
    ///
    /// Gives visibility to internalimplementation
    template <int NRefComps,
	      typename MDel,
	      typename PosInRef>
    using MergedDelimsInRef=
      decltype(mergedDelimsInRef<NRefComps>(MDel{},PosInRef{}));
  }
  
  // Base type to qualify as Adder
  DEFINE_BASE_TYPE(Adder);
  
  /// Class to take the added of a SmET
  template <typename _Ref1,                                    // First addendum type
	    typename _Ref2,                                    // Second addendum type
	    typename TK1=typename RemoveReference<_Ref1>::Tk,  // Tens Kind of the first addendum type
	    typename TK2=typename RemoveReference<_Ref2>::Tk>  // Tens Kind of the second addendum type
  class Adder :
    public BaseAdder,                         // Inherit from BaseAdder to detect in expression
    public BinarySmET<Adder<_Ref1,_Ref2>>,    // Inherit from BinarySmET
    public ConstrainAreTensKinds<TK1,TK2>     // Constrain type TK1 and TK2 to be a TensKind
  {
    STATIC_ASSERT_IS_SMET(RemoveReference<_Ref1>);
    STATIC_ASSERT_IS_SMET(RemoveReference<_Ref2>);
    
  public:
    
    /// Returns the size of a component
    template <typename TC>
    int compSize() const
    {
      // Returns the size if it's in the first Tk or in the second
      if constexpr(tupleHasType<TC,typename TK1::types>)
	 return ref1.template compSize<TC>();
      else
	 return ref2.template compSize<TC>();
    }
    
    PROVIDE_BINARY_SMET_REFS;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    /// TensorKind of the result
    ///
    /// \todo Improve, the return type could be decided studying what
    /// suits best the computational
    using Tk=
      BlendTensKinds<TK1,TK2>;
    
    /// Provides the MergedDelim attribute for Refs 1 and 2
#define PROVIDE_POS_OF_ADDEND_TC_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(ID) \
    /*! Position of the \c TensComp of Addend ## ID in Res Tk */	\
    using posOfAddend ## ID ## TcInResTk=				\
      posOfTypesNotAsserting<typename Tk::types,typename TK ## ID::types>; \
									\
    /*! Position of the \c TensComp of Addend ## ID present in Res Tk */ \
    using posOfAddend ## ID ## PresTcInResTk=				\
      FilterVariadicClassPos<IsNonNegative,posOfAddend ## ID ## TcInResTk>; \
    									\
    /*! Merged delimiters of Ref ## ID according to MD */		\
    template <typename MD> /* Required merging delimiters */		\
    using MergedDelims ## ID=						\
      PairOfTensKindMergeability::MergedDelimsInRef<TK ## ID::nTypes,MD,posOfAddend ## ID ## TcInResTk>
    
    PROVIDE_POS_OF_ADDEND_TC_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(1);
    PROVIDE_POS_OF_ADDEND_TC_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(2);

#undef PROVIDE_POS_OF_ADDEND_TC_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF
       
    // Provide component-mergeabilty
    PROVIDE_MERGEABLE_COMPS(/*! Defer the mergeability to the internal implementation*/,
			    PairOfTensKindMergeability::template CompsMergeability<
			    typename RemoveReference<Ref1>::MergeableComps,
			    typename RemoveReference<Ref2>::MergeableComps,
			    posOfAddend1TcInResTk,
			    posOfAddend2TcInResTk>);
    
    // Returns a component-merged version
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Merge appropriately the two references and returns their sum */,
    				  return
				  ref1.template getMergedCompsView<MergedDelims1<Is>>()+
				  ref2.template getMergedCompsView<MergedDelims2<Is>>(););
    
    PROVIDE_BINARY_SMET_SIMPLE_CREATOR(Adder);
  };
  
  // Check that a test Adder is a BinarySmET
  namespace CheckAdderIsBinarySmet
  {
    /// Tensor comp for test
    using MyTc=
      TensComp<double,1>;
    
    /// Tensor kind to be tested
    using MyTk=
      TensKind<MyTc>;
    
    /// Tensor to be tested
    using MyT=
      Tens<MyTk,double>;
    
    STATIC_ASSERT_IS_BINARY_SMET(Adder<MyT,MyT>);
  }
  
  // Build Adder from add
  SIMPLE_BINARY_SMET_BUILDER(add,Adder);
  
  /// Implement smet1+smet2
  template <typename T1,              // Type of the first expression
	    typename T2,              // Type of the second expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T1> and isSmET<T2>)>
  DECLAUTO operator+(T1&& smet1,      ///< First addend
		     T2&& smet2)      ///< Second addend
  {
    return add(forw<T1>(smet1),forw<T2>(smet2));
  }
}

#endif
