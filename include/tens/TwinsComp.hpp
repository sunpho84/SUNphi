#ifndef _TWINSCOMP_HPP
#define _TWINSCOMP_HPP

/// \file TwinsComp.hpp
///
/// \brief Header file to define twins TensComp
///
/// The twin component of a TensComp is another TensComp. A pair of
/// TwinComp is used to distinguish row and column component of
/// matrices.

#include <metaprogramming/Tuple.hpp>
#include <tens/TensComp.hpp>
#include <tex/BaseTEx.hpp>
#include <utility/Unused.hpp>

namespace SUNphi
{
  /// Determine if the TensComp has a twin
  ///
  /// Default for a generic TensComp: false
  template <class T,                         // Type to declare not twinned
	    class=ConstrainIsTensComp<T>>    // Constrain the T to be a TensComp
  constexpr bool hasTwin=false;
  
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
  constexpr bool hasTwin<T1>			\
  =true;					\
  						\
  MAYBE_UNUSED(hasTwin<T1>);			\
						\
  /*! Declare that T2 has a twin */		\
  template <>					\
  constexpr bool hasTwin<T2>			\
  =true;					\
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
}

#endif
