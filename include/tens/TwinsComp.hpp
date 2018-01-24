#ifndef _TWINSCOMP_HPP
#define _TWINSCOMP_HPP

/// \file TwinsComp.hpp
///
/// \brief Header file to define twins TensComp
///
/// The twin component of a TensComp is another TensComp. A pair of
/// TwinComp is used to distinguish row and column component of
/// matrices.

#include <ints/IntSeqInsert.hpp>
#include <tens/TensComp.hpp>
#include <tex/BaseTEx.hpp>
#include <tuple/TupleClass.hpp>
#include <tuple/TupleOrder.hpp>
#include <utility/Unused.hpp>

namespace SUNphi
{
  /// Determine if the TensComp has a twin
  ///
  /// Default for a generic TensComp: false
  template <class T,                         // Type to declare not twinned
	    class=ConstrainIsTensComp<T>>    // Constrain the T to be a TensComp
  [[ maybe_unused ]]
  constexpr bool hasTwin=
    false;
  
  /// Specify the twin component of a given TensComp
  ///
  /// Internal implementation for generic case, returning the input
  template <class T,                         // Type of which we want to declare the twin
	    class=ConstrainIsTensComp<T>>    // Constrain T to be a TensComp
  struct _TwinCompOf
  {
    /// Twinned type: the same of input type
    using type=T;
  };
  
  /// Specify the twin component of a given TensComp
  template <class T,                            // Type of which we want to declare the twin
	    class=ConstrainIsTensComp<T>>       // Constrain T to be a TensComp
  using TwinCompOf=typename _TwinCompOf<T>::type;
  
  /// Declare a pair of TensComp twins one of the other
#define DECLARE_TENS_COMPS_ARE_TWIN(T1,T2)	\
  						\
  /*! Declare that T1 has a twin */		\
  template <>					\
  constexpr bool hasTwin<T1> =			\
    true;					\
  						\
  MAYBE_UNUSED(hasTwin<T1>);			\
						\
  /*! Declare that T2 has a twin */		\
  template <>					\
  constexpr bool hasTwin<T2> =			\
    true;					\
						\
  MAYBE_UNUSED(hasTwin<T2>);			\
  						\
  /*! Declare the twinned type of T1 */		\
  template <>					\
  struct _TwinCompOf<T1>			\
  {						\
    /*! Twinned type */				\
    using type=ConstrainIsTensComp<T2>::type;	\
  };						\
						\
  /*! Declare the twinned type of T2 */		\
  template <>					\
  struct _TwinCompOf<T2>			\
  {						\
    /*! Twinned type */				\
    using type=ConstrainIsTensComp<T1>::type;	\
  }
  
  /// If the TensComp TC is not present in the TensKind of TEX, returns the twin
  template <typename Tc,   // Tensor Component searched
	    typename TEX,  // Type of the expression where to search
 	    typename=ConstrainIsTensComp<Tc>, // Constrain Tc to be a TensComp
 	    typename=ConstrainIsTEx<TEX>,     // Constrain TEX to be a TEx
	    typename TK=typename Unqualified<TEX>::Tk,     // Tens Kind of the TEx
	    typename TK_TYPES=typename TK::types,          // Types of the tensor kind
	    bool Has=tupleHasType<Tc,TK_TYPES>>            // Check if the tuple has type
  using CompOrTwinned=Conditional<Has,Tc,TwinCompOf<Tc>>;
  
  /// Insert in the IntSeq the position of all true twinned types
  ///
  /// Internal implementation
  template <int Pos,        // Searched position
	    int...Ints,     // IntSeq parameters
	    typename...T>   // TensComp
  DECLAUTO _InsertTrueTwinnedPosOfTuple(const IntSeq<Ints...>& in, ///< Input IntSeq
					const Tuple<T...>& types)  ///< Input types
  {
      static_assert((isTensComp<T> & ...),"Supported only for tensComp");
      
      // Input IntSeq
      using In=IntSeq<Ints...>;
      // Input Tuple
      using Types=Tuple<T...>;
      // Twinned Tuple
      using TwTypes=Tuple<TwinCompOf<T>...>;
      //
      // Finished the types to check
      if constexpr(sizeof...(T)<=Pos)
	 return in;
      else
	{
	  // Type in position Pos
	  using Comp=Unqualified<TupleElementType<Pos,Types>>;
	  // Twinned comp
	  using TwComp=TwinCompOf<Comp>;
	  
	  // Check if it is a real twin
	  constexpr bool isTrueTwin=not isSame<TwComp,Comp>;
	  // Check if twinned Tuple had original comp
	  constexpr bool hadTwinned=tupleHasType<Comp,TwTypes>;
	  
	  // Result type
	  using Out=Conditional<isTrueTwin and hadTwinned,
		                InsertIntSeqInOrderedIntSeq<IntSeq<Pos,Pos+1>,   // Position to insert
							    In,                  // List where to insert
							    IntSeq<0,0>,         // Do not shift after inserting
							    true>,               // Ignore insertion if present
				In>;
	  
	  return _InsertTrueTwinnedPosOfTuple<Pos+1>(Out{},types);
	}
    }
  
  /// Declares the twin types of a TensComp
  ///
  /// Forward declaration, real implementation created by the macro
  /// which defines a twin pair
  template <typename T>
  struct TwinTensCompOf;
  
  /////////////////////////////////////////////////////////////////
  
  /// Insert in the IntSeq the position of all true twinned types
  ///
  /// A true twinned type is a type for which the twin is different
  /// from itself, and is present in the list
  template <typename Is,  // Integer to be extended
	    typename Tp>  // Tuple to analyze
  using InsertTrueTwinnedPosOfTuple=
    decltype(_InsertTrueTwinnedPosOfTuple<0>(Is{},Tp{}));
}

#endif
