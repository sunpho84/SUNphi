#ifndef _INTSEQ_HPP
#define _INTSEQ_HPP

/// \file IntSeq.hpp
///
/// \brief Implements a struct holding a sequence of integers.

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Sum of all integers
  ///
  template <int Head=0,int...Tail>
  constexpr int hSum=Head+hSum<Tail...>;
  
  /// Sum of all integers, unary case
  ///
  template <int Head>
  constexpr int hSum<Head> =Head;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// A struct holding a sequence of integer (similar to stdlib).
  ///
  /// The \c integers are held as parameters of the class. To
  /// intercept them, use the class in a context in which the integer
  /// list are deduced as template parameter, as in this example:
  ///
  /// \code
  /// template <class T>
  /// class PutIntoArrays
  /// {
  /// };
  ///
  /// template <int...Ints>
  /// class PutIntoArrays<IntSeq<Ints...>>
  /// {
  ///    std::array<int,IntSeq<Ints...>::Size> arr{{Ints...}};
  /// };
  /// \endcode
  ///
  /// \tparam Ints the list of integer
  ///
  template <int...Ints>
  struct IntSeq
  {
    static constexpr int size=sizeof...(Ints); ///< Length of the sequence of integer
    static constexpr int hSum=SUNphi::hSum<Ints...>; ///< Sum of all elements
    
    template <int I> ///< Defines a integer sequence incremented by I
    using Add=IntSeq<(Ints+I)...>;
    
    template <int I> ///< Defines a integer sequence decreased by I
    using Sub=Add<-I>;
    
    template <int I> ///< Define a integer sequence inflated by I
    using Mul=IntSeq<(Ints*I)...>;
    
    template <int I> ///< Define a integer sequence decreased by I
    using Div=TypeIf<I!=0,Mul<1/I>>;
  };
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Internal implementation of sequence-of-integer catter.
  ///
  /// Dummy prototype to be specialized with actual case.
  ///
  template <class...T>
  struct IntSeqCatImpl;
  
  /// Internal implementation of sequence-of-integer catter.
  ///
  /// Takes into account catting a single instance of IntSeq, useful
  /// also to terminate the catter. Called also when the general case
  /// runs out of argument.
  ///
  /// Example:
  /// \code
  /// using Seq=IntSeq<1,2,3,4>;
  /// IntSeqCatImpl<Seq>::type test; //IntSeq<1,2,3,4>
  /// \endcode
  ///
  template <int...Ints>
  struct IntSeqCatImpl<IntSeq<Ints...>>
  {
    typedef IntSeq<Ints...> type; ///< Internally mapped type
  };
  
  /// Internal implementation of sequence-of-integer catter.
  ///
  /// Takes into account the general case of an arbitary number of
  /// IntSeq, instantiating recursively the binary cat types.
  ///
  /// Example:
  /// \code
  /// using Seq=IntSeq<1,2,3,4>;
  /// IntSeqCatImpl<Seq,Seq>::type test; //IntSeq<1,2,3,4,1,2,3,4>
  /// \endcode
  ///
  template <int...Ints1,int...Ints2,class...T>
  struct IntSeqCatImpl<IntSeq<Ints1...>,IntSeq<Ints2...>,T...>
  {
    using Nested=IntSeq<Ints1...,Ints2...>;                 ///< Binary cat \c Ints1..., \c Ints2..., separated for clarity
    typedef typename IntSeqCatImpl<Nested,T...>::type type; ///< Result of catting the whole list \c Ints1, \c Ints2,...
  };
  
  /// Sequence-of-integer catter.
  ///
  /// Wraps the implementation to avoid writing "type"
  ///
  template <class...T>
  using IntSeqCat=typename IntSeqCatImpl<T...>::type;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Internal implementation, recursively calling itself until 0 or 1
  ///
  template <int Max>
  struct IntsUpToImpl
  {
    static constexpr int half=Max/2;                                                   ///< Used to split the list
    using type=IntSeqCat<typename IntsUpToImpl<half>::type,
			 typename IntsUpToImpl<Max-half>::type::template Add<half>>;   ///< Internal type holding the two halves
  };
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Implement the terminator, considering a sequence up to 0 (empty)
  ///
  template <>
  struct IntsUpToImpl<0>
  {
    using type=IntSeq<>; ///< Empty list
  };
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Implement the terminator, considering a sequence up to 1 (including only 0)
  ///
  template <>
  struct IntsUpToImpl<1>
  {
    using type=IntSeq<0>; ///< Trivial list
  };
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Wraps the internal definition
  ///
  template <int Max>
  using IntsUpTo=typename IntsUpToImpl<Max>::type;
  
  ///////////////////////////////////////////////////////////////////////
  
  /// Defines a sequence of integer with offset and stride (up-open interval)
  ///
  /// This is achieved using the Add and Mul from the IntSeq list
  ///
  template <int Min,int Shift,int Max>
  struct RangeSeqImpl
  {
    static_assert(Shift,"Shift must be non-zero"); //assert if Shift is zero
    
    static constexpr bool nonNull=(Max>Min);                      ///< Mask used to set to zero the Range parameters
    static constexpr int normalizedMax=nonNull*((Max-Min)/Shift); ///< Maximal value of the normalized range
    static constexpr int stride=nonNull*Shift;                    ///< Stride among entries
    static constexpr int offset=nonNull*Min;                      ///< Offset of the sequence
    using type=typename IntsUpTo<normalizedMax>
      ::template Mul<stride>
      ::template Add<offset>;                                     ///< Shifted-strided interval
  };
  
  /// Defines a sequence of integer with offset and stride (up-open interval)
  ///
  /// Example:
  ///
  /// \code
  /// typedef RangeSeq<2,3,8> Range; //IntSeq<2,5>
  /// \endcode
  ///
  template <int Min,int Shift,int Max>
  using RangeSeq=typename RangeSeqImpl<Min,Shift,Max>::type;
}

#endif
