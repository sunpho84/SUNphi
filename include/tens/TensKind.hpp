#ifndef _TENSKIND_HPP
#define _TENSKIND_HPP

/// \file TensKind.hpp
///
/// \brief Defines the Tensor Kind
///
/// The tensor kind defines the list of components that form a tensor

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <array>


#include <ints/IntSeqInsert.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <tens/TensComp.hpp>
#include <tens/TwinsComp.hpp>
#include <tuple/TupleElements.hpp>
#include <tuple/TupleOrder.hpp>

namespace SUNphi
{
  /// Dynamic sizes of a Tens
  ///
  /// \todo the array must be replaced with a tuple, whose types
  /// must be deduced when instatiating the struct, such that int or
  /// long int or whatever is appropriately used!
  template <int N>
  using DynSizes=std::array<int,N>;
  
  /// Defines the BaseTensKind type traits
  DEFINE_BASE_TYPE(TensKind);
  
  /// \cond SUNphi_INTERNAL
  /// TensKind, forward definition
  template <class...T>
  class TensKind;
  /// \endcond
  
  /// Dynamic sizes of a Tens
  ///
  /// \todo the array must be replaced with a tuple, whose types
  /// must be deduced when instatiating the struct, such that int or
  /// long int or whatever is appropriately used!
  template <int N>
  using DynSizes=std::array<int,N>;
  
  // Provide a checker for compSize presence
  DEFINE_HAS_MEMBER(compSize);
  
  /// Returns whether T is a dynamic component
  template <typename T>
  [[ maybe_unused ]]
  static constexpr bool isDynamic=T::isDynamic;
  
  /// Defines a TensKind type from a Tuple type
  /// Static assert if TC is not dynamic
#define STATIC_ASSERT_IS_DYNAMIC(TC)					\
  static_assert(isDynamic<TC>,"Error, Tens Comp is not dynamic")
  
  /// Forces type Tc to be a dynamic TensComp
  template <typename Tc>
  struct ConstrainIsDynamic
  {
    STATIC_ASSERT_IS_DYNAMIC(Tc);
  };
  
  DEFINE_VARIADIC_TYPE_FROM_TUPLE(TensKind);
  
  /// Tensor Kind used to define the structure of a tensor
  ///
  /// The tensor kind defines the list of components of a tensor. It
  /// is used to the define the underlying set of components of a \c
  /// TensorStorage, or the returned type of a SmET
  template <class...T>
  class TensKind : public BaseTensKind
  {
    // Check that all types are TensComp
    static_assert(IntSeq<isTensComp<T>...>::hSum==sizeof...(T),"Cannot define a TensKind for types not inheriting from TensComp");
    
    /// Check that all types are different
    STATIC_ASSERT_TUPLE_TYPES_ARE_ALL_DIFFERENT(Tuple<T...>);
    
    /// An integer sequence defining whether the types are dynamic or not
    typedef IntSeq<(T::size==DYNAMIC)...> AreDynamic;
    
    /// Position of dynamical components
    ///
    /// Internal implementation
    template <int NScanned,       // Number of components scanned so far
	      int...CompsPos>  // Dynamical positions found so far
    static constexpr DECLAUTO _DynCompsPos(const IntSeq<CompsPos...>&)
    {
      if constexpr(NScanned==sizeof...(T))
	// Returns
	return intSeq<CompsPos...>;
      else
	if constexpr(AreDynamic::template element<NScanned>)
	  // Nest appending
	  return _DynCompsPos<NScanned+1>(intSeq<CompsPos...,NScanned>);
	else
	  // Nest without appending
	  return _DynCompsPos<NScanned+1>(intSeq<CompsPos...>);
    }
    
  public:
    
    /// Position of dynamical components
    using DynCompsPos=decltype(_DynCompsPos<0>(intSeq<>));
    
    /// Number of dynamical components
    static constexpr int nDynamic=AreDynamic::hSum;
    
    /// Check if the type is fully static
    static constexpr bool isFullyStatic=(nDynamic==0);
    
    /// Tuple containing all types
    typedef Tuple<T...> types;
    
    /// Number of types of the kind
    static constexpr int nTypes=sizeof...(T);
    
    /// Returns the position of a dynamical size
    template <typename TC,
	      typename=ConstrainIsTensComp<TC>,
	      typename=ConstrainIsDynamic<TC>>
    static constexpr TypeIf<isTensComp<TC> and isDynamic<TC>,int>
    dynCompPos=
      AreDynamic::template hSumFirst<posOfType<TC,types>>;
    
    // /// Position of a given type
    // template <class Tf>
    // static constexpr int posOfType=posOfType<Tf,Tuple<T...>>;
    
    // /// Get all types before one
    // template <class Tab>
    // using AllBeforeType=TensKindFromTuple<decltype(getHead<Tab>(Types{}))>;
    
    /// Get all types but one
    template <class Tab>
    using AllButType=
      TensKindFromTuple<decltype(getAllBut<Tab>(types{}))>;
    
    /// Return the position of the first component needed to vectorize
    ///
    /// Internal implementation, escaping when last checkable
    /// component is reached
    ///
    /// \todo Add a lenghty description, this is a complicated piece
    /// of code! The scope is not immediate to get
    template <typename F,                                                              // Fundamental type
	      int Pos=nTypes-1,                                                        // Component searched at the moment
	      int InVectorizingSize=1,                                                 // Size of the currently scanned vectorizable components
	      bool IsLastCheckable=(Pos==0),                                           // Determine if this is the last checkable
	      int NextPos=(IsLastCheckable?Pos:(Pos-1)),                               // Determine the position of next component
	      typename G=RemoveReference<decltype(get<Pos>(types{}))>,                 // Get the type of the component under exam
	      int Size=G::size,                                                        // Size of current component
	      int OutVectorizingSize=InVectorizingSize*Size,                           // Returned vectorized size, including current component
	      bool EnoughToVectorize=canBeSizeOfSIMDVector<F>(OutVectorizingSize),     // Check if the accumulated size is enough to vectorize
	      bool CompIsVectorizing=G::template isVectorizable<F>,                    // Check if the current componente is vectorizable
	      bool FallBack=(not CompIsVectorizing) or IsLastCheckable>                // Check if we need to fallback
    struct _firstVectorizingComp
    {
      ///Provides the result of the calculation
      constexpr static int value=
	FallBack?                                              // If we need to fallback,
	-1:                                                    // returns -1, otherwise
	(EnoughToVectorize?                                    // if we have accumulated enough components,
	 Pos:                                                  // we return current position, otherwise
	 Conditional<IsLastCheckable,                          // if this is the last checkable component,
	  std::integral_constant<int,-1>,                      // returns -1 otherwise
 	  _firstVectorizingComp<F,NextPos,OutVectorizingSize>  // we go to previous component.
	 >::value);                                            // Value is obtained through a Conditional to avoid infinite recursion
    };
    
    /// Return the position of the first component needed to vectorize
    ///
    /// If no factorization is possible, returns -1
    ///
    /// \todo Add the possibility that the accumulated size is more
    /// than needed, so we can split the outermost component to allow
    /// for other kind of optimization
    ///
    /// \todo Include a variation of the vectorization according to
    /// the kind of SIMD vector
    template <typename F>                 // Fundamental type
    constexpr static int firstVectorizingComp=_firstVectorizingComp<F>::value; // Get the value of the internal implementation
    
    // /// Get all types after one
    // template <class Tab>
    // using AllAfterType=TensKindFromTuple<decltype(getTail<Tab>(Types{}))>;
    
    /// Get the twinned (transposed) type
    using Twinned=TensKind<TwinCompOf<T>...>;
    
    /// Reports whether a component is Matricial or not
    using isMatrixComp=IntSeq<(hasTwin<T> and tupleHasType<T,typename Twinned::types>)...>;
    
    /// Insert in the IntSeq Is the points where true twinned types are present
    template <typename Is>
    using InsertTrueTwinnedPos=
      InsertTrueTwinnedPosOfTuple<Is,types>;
    
    /// Maximal value of the index, restricted to the statical components
    static constexpr int maxStaticIdx=
      IntSeq<(T::size>=0 ? T::size : 1)...>::hMul;
    
    /////////////////////////////////////////////////////////////////
    
    /// Maximal total submultiple of a list of components
    template <int...Ints>
    static constexpr int tensCompsListTotMaxKnownSubMultiple=
      IntSeq<TupleElementType<Ints,types>::maxKnownSubMultiple...>::hMul;
    
    /// Total size of a list of components
    ///
    /// If any component is dynamic, returns DYNAMIC, otherwise the product
    template <int...Ints>
    static constexpr int tensCompsListTotSize=
      (TupleElementType<Ints,types>::isDynamic | ...) ? DYNAMIC :
      IntSeq<TupleElementType<Ints,types>::size...>::hMul;
    
    /////////////////////////////////////////////////////////////////
    
    /// Create a TensComp merging the components IComps
    ///
    /// Forward definition
    template <typename Is>
    struct TensCompsListMerged;
    
    /// Create a TensComp merging the components IComps
    template <int...IComps>                  // Index of the components to merge
    struct TensCompsListMerged<IntSeq<IComps...>>
    {
      /// First component of the groups
      static constexpr int firstComp=IntSeq<IComps...>::template element<0>;
      
      /// Check if we are really merging something
      static constexpr bool realMerge=(sizeof...(IComps)>1);
      
      /// Returns a tuple containing the merged components of group I
      using MergedComps=decltype(getIndexed(intSeq<IComps...>,
					    types{}));
      
      /// Product of all the max known submultiple
      static constexpr int totMaxKnonwSubMultiple=tensCompsListTotMaxKnownSubMultiple<IComps...>;
      
      /// Returns the totals size of the group
      static constexpr int totSize=tensCompsListTotSize<IComps...>;
      
      /// Resulting type
      using type=Conditional<realMerge,
			     // Merged type if the group was larger than 1
			     TensComp<MergedComps,totSize,totMaxKnonwSubMultiple>,
			     // Otherwise return the type itself
			     TupleElementType<firstComp,types>>;
    };
    
    /// Struct used to merge the components of the TensKind
    ///
    /// Forward definition
    template <typename Is,
	      typename Ir>
    struct _Merged;
    
    /// Determines whether an IntSeq is a valid CastMerge type for the TensKind
    template <typename Is>
    static constexpr bool isValidCompMerge=
      isOrderedIntSeq<Is> and
      (Is::template element<0> ==0) and
      (Is::last == nTypes);
    
    /// Helper to constrain the cast
    template <typename Is>
    struct ConstrainIsValidCompMerge
    {
      static_assert(isOrderedIntSeq<Is>,"Not an ordered IntSeq");
      static_assert(Is::template element<0> ==0, "First element not zero");
      static_assert(Is::last==nTypes,"Last element not equal to the number of components of the TensKind");
    };
    
    /// Struct used to merge the components of the TensKind
    template <int...IDelims,
	      int...IGroups>
    struct _Merged<IntSeq<IDelims...>,IntSeq<IGroups...>> :
      public ConstrainIsValidCompMerge<IntSeq<IDelims...>>
    {
      /// Delimiters
      using Delims=IntSeq<IDelims...>;
      
      /// Range associated with merge grpup I
      template <int I>                  // Index of the group of components to merge
      using Range=
	RangeSeq<
	Delims::template element<I>,    // Start
	1,                              // Offset
	Delims::template element<I+1>>; // End
      
      /// Create a TensComp merging the components of group I
      template <int I>      // Index of the group of components to merge
      using TensCompsGroupMerged=
	typename TensCompsListMerged<Range<I>>::type;
      
      /// TensKind resulting after merging all groups
      using type=TensKind<TensCompsGroupMerged<IGroups>...>;
    };
    
    /// Merged components according to IntSeq Is
    template <typename Is>
    using Merged=
      typename _Merged<Is,IntsUpTo<Is::size-1>>::type;
  };
}

#endif
