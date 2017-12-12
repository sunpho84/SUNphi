#ifndef _INTSEQ_HPP
#define _INTSEQ_HPP

/// \file IntSeq.hpp
///
/// \brief Implements a struct holding a sequence of integers.

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Sum of all integers
  template <int Head=0,int...Tail>
  constexpr int hSum=Head+hSum<Tail...>;
  
  /// Sum of all integers, unary case
  template <int Head>
  constexpr int hSum<Head> =Head;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Product of all integers
  template <int Head=0,int...Tail>
  constexpr int hMul=Head*hMul<Tail...>;
  
  /// Product of all integers, unary case
  template <int Head>
  constexpr int hMul<Head> =Head;
  
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
  template <int...Ints>
  struct IntSeq
  {
    /// Length of the sequence of integer
    static constexpr int size=sizeof...(Ints);
    
    /// Sum of all elements
    static constexpr int hSum=SUNphi::hSum<Ints...>;
    
    /// Product of all elements
    static constexpr int hMul=SUNphi::hMul<Ints...>;
    
    /// Defines a integer sequence incremented by I
    template <int I>
    using Add=IntSeq<(Ints+I)...>;
    
    /// Defines a integer sequence decreased by I
    template <int I>
    using Sub=Add<-I>;
    
    /// Define a integer sequence inflated by I
    template <int I>
    using Mul=IntSeq<(Ints*I)...>;
    
    /// Define a integer sequence decreased by I
    template <int I>
    using Div=TypeIf<I!=0,Mul<1/I>>;
  };
  
  /////////////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Internal implementation of sequence-of-integer catter.
    ///
    /// Dummy prototype to be specialized with actual case.
    template <class...T>
    struct IntSeqCat;
    
    /// Internal implementation of sequence-of-integer catter.
    ///
    /// Takes into account catting a single instance of IntSeq, useful
    /// also to terminate the catter. Called also when the general case
    /// runs out of argument.
    ///
    /// Example:
    /// \code
    /// using Seq=IntSeq<1,2,3,4>;
    /// IntSeqCat<Seq>::type test; //IntSeq<1,2,3,4>
    /// \endcode
    template <int...Ints>
    struct IntSeqCat<IntSeq<Ints...>>
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
    /// IntSeqCat<Seq,Seq>::type test; //IntSeq<1,2,3,4,1,2,3,4>
    /// \endcode
    template <int...Ints1,int...Ints2,class...T>
    struct IntSeqCat<IntSeq<Ints1...>,IntSeq<Ints2...>,T...>
    {
      /// Binary cat \c Ints1..., \c Ints2..., separated for clarity
      using Nested=IntSeq<Ints1...,Ints2...>;
      
      /// Result of catting the whole list \c Ints1, \c Ints2,...
      typedef typename IntSeqCat<Nested,T...>::type type;
    };
  }
  
  /// Sequence-of-integer catter.
  ///
  /// Wraps the implementation to avoid writing "type"
  template <class...T>
  using IntSeqCat=typename Impl::IntSeqCat<T...>::type;
  
  /////////////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Defines a sequence of integer up to Max (excluded)
    ///
    /// Internal implementation, recursively calling itself until 0 or 1
    template <int Max>
    struct IntsUpTo
    {
      /// Used to split the list
      static constexpr int half=Max/2;
      
      /// Internal type holding the two halves
      using type=IntSeqCat<typename IntsUpTo<half>::type,
			   typename IntsUpTo<Max-half>::type::template Add<half>>;
    };
    
    /// Defines a sequence of integer up to Max (excluded)
    ///
    /// Implement the terminator, considering a sequence up to 0 (empty)
    template <>
    struct IntsUpTo<0>
    {
      /// Empty list
      using type=IntSeq<>;
    };
    
    /// Defines a sequence of integer up to Max (excluded)
    ///
    /// Implement the terminator, considering a sequence up to 1 (including only 0)
    template <>
    struct IntsUpTo<1>
    {
      /// Trivial list
      using type=IntSeq<0>;
    };
  }
  
  /// Defines a sequence of integer up to Max (excluded)
  ///
  /// Wraps the internal definition
  template <int Max>
  using IntsUpTo=typename Impl::IntsUpTo<Max>::type;
  
  ///////////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Defines a sequence of integer with offset and stride (up-open interval)
    ///
    /// This is achieved using the Add and Mul from the IntSeq list
    template <int Min,int Shift,int Max>
    struct RangeSeq
    {
      static_assert(Shift,"Shift must be non-zero"); //assert if Shift is zero
      
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
  }
  
  /// Defines a sequence of integer with offset and stride (up-open interval)
  ///
  /// Example:
  ///
  /// \code
  /// typedef RangeSeq<2,3,8> Range; //IntSeq<2,5>
  /// \endcode
  template <int Min,int Shift,int Max>
  using RangeSeq=typename Impl::RangeSeq<Min,Shift,Max>::type;
}

#endif
