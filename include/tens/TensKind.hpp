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

#include <metaprogramming/IntSeq.hpp>
#include <metaprogramming/Tuple.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <tens/TensComp.hpp>
#include <tens/TwinsComp.hpp>

namespace SUNphi
{
  /// Defines the BaseTensKind type traits
  DEFINE_BASE_TYPE(TensKind);
  
  /// \cond SUNphi_INTERNAL
  /// TensKind, forward definition
  template <class...T>
  class TensKind;
  /// \endcond
  
  /// Defines a TensKind type from a Tuple type
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
    static_assert(IntSeq<IsTensComp<T>...>::hSum==sizeof...(T),"Cannot define a TensKind for types not inheriting from TensComp");
    
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
    
    /// Maximal value of the index, restricted to the statical components
    static constexpr int maxStaticIdx=
      IntSeq<(T::size>=0 ? T::size : 1)...>::hMul;
  };
}

#endif
