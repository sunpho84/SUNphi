#ifndef _RANGES_HPP
#define _RANGES_HPP

/// \file Ranges.hpp
///
/// \brief Provide range creators

#include <ints/IntSeq.hpp>
#include <ints/IntSeqCat.hpp>

namespace SUNphi
{
  /////////////////////////////////////////////////////////////////////////
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Internal implementation, recursively calling itself until 0 or 1
  template <int Max>
  struct _IntsUpTo
  {
    /// Used to split the list
    static constexpr int half=Max/2;
    
    /// Internal type holding the two halves
    using type=IntSeqCat<typename _IntsUpTo<half>::type,
			 typename _IntsUpTo<Max-half>::type::template Add<half>>;
  };
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Implement the terminator, considering a sequence up to 0 (empty)
  template <>
  struct _IntsUpTo<0>
  {
    /// Empty list
    using type=IntSeq<>;
  };
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Implement the terminator, considering a sequence up to 1 (including only 0)
  template <>
  struct _IntsUpTo<1>
  {
    /// Trivial list
    using type=IntSeq<0>;
  };
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Wraps the internal definition
  template <int Max>
  using IntsUpTo=typename _IntsUpTo<Max>::type;
  
  ///////////////////////////////////////////////////////////////////////
  
  /// Defines a sequence of integer with offset and stride (up-open interval)
  ///
  /// This is achieved using the Add and Mul from the IntSeq list
  template <int Min,
	    int Shift,
	    int Max>
  struct _RangeSeq
  {
    //assert if Shift is zero
    static_assert(Shift,"Shift must be non-zero");
    
    /// Mask used to set to zero the Range parameters
    static constexpr bool nonNull=(Max>Min);
    
    /// Maximal value of the normalized range
    static constexpr int normalizedMax=nonNull*((Max-Min)/Shift);
    
    /// Stride among entries
    static constexpr int stride=nonNull*Shift;
    
    /// Offset of the sequence
    static constexpr int offset=nonNull*Min;
    
    /// Shifted-strided interval
    using type=typename SUNphi::IntsUpTo<normalizedMax>
      ::template Mul<stride>
      ::template Add<offset>;
  };
  
  /// Defines a sequence of integer with offset and stride (up-open interval)
  ///
  /// Example:
  ///
  /// \code
  /// typedef RangeSeq<2,3,8> Range; //IntSeq<2,5>
  /// \endcode
  template <int Min,   // Starting point
	    int Shift, // Stride
	    int Max>   // End point (not included)
  using RangeSeq=typename _RangeSeq<Min,Shift,Max>::type;
  
  /// Create a RangeSeq with given types
  template <int Min,   // Starting point
	    int Shift, // Stride
	    int Max>   // End point (not included)
  [[ maybe_unused ]]
  constexpr DECLAUTO rangeSeq=
    RangeSeq<Min,Shift,Max>{};
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a IntSeq of size Num, all containing Entry as entry
  template <int Num,   // Number of components
	    int Val>   // Value to be used
  using IntSeqOfSameNumb=
    typename IntsUpTo<Num>::
    template Mul<0>::
    template Add<Val>;
}

#endif
