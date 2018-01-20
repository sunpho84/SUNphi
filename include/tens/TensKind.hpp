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
  /// TensorStorage, or the returned type of a Template Expression
  template <class...T>
  class TensKind : public BaseTensKind
  {
    // Check that all types are TensComp
    static_assert(IntSeq<isTensComp<T>...>::hSum==sizeof...(T),"Cannot define a TensKind for types not inheriting from TensComp");
    
    /// Check that all types are different
    STATIC_ASSERT_TUPLE_TYPES_ARE_ALL_DIFFERENT(Tuple<T...>);
    
    /// An integer sequence defining whether the tuypes are dynamic or not
    typedef IntSeq<(T::size==DYNAMIC)...> AreDynamic;
    
  public:
    
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
    dynCompPos=AreDynamic::template hSumFirst<posOfType<TC,types>>;
    
    // /// Position of a given type
    // template <class Tf>
    // static constexpr int posOfType=posOfType<Tf,Tuple<T...>>;
    
    // /// Get all types before one
    // template <class Tab>
    // using AllBeforeType=TensKindFromTuple<decltype(getHead<Tab>(Types{}))>;
    
    /// Get all types but one
    template <class Tab>
    using AllButType=TensKindFromTuple<decltype(getAllBut<Tab>(types{}))>;
    
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
  };
}

#endif
