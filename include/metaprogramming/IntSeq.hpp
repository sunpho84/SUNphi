#ifndef _INTSEQ_HPP
#define _INTSEQ_HPP

/// \file IntSeq.hpp
///
/// \brief Implements a struct holding a sequence of integers.

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Sum of all integers
  template <int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hSum=Head+hSum<Tail...>;
  
  /// Sum of all integers, unary case
  template <int Head>
  constexpr int hSum<Head> =Head;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Sum of all integers up to I (excluded)
  template <int I,
	    int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hSumFirst=(I>0)*Head+hSumFirst<I-1,Tail...>;
  
  /// Sum of all integers up to I (excluded),unary case
  template <int I,
	    int Head>
  constexpr int hSumFirst<I,Head> =(I>0)*Head;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Returns the position of the first non-occurrency of I
  ///
  /// Internal implementaton
  template <int I,       // Element to search
	    int Pos,     // Position currently reached
	    int Head,    // First component to be searched
	    int...Ints>  // Other components
  constexpr int _firstNon()
  {
    if constexpr(I!=Head or sizeof...(Ints)==0)
      return Pos;
    else
      return _firstNon<I,Pos+1,Ints...>();
  }
  
  /// Returns the position of the first non-occurrency of I
  ///
  /// General case
  template <int I,       // Element to search
	    int...Ints>  // Components
  [[ maybe_unused ]]
  constexpr int firstNon=_firstNon<I,0,Ints...>();
  
  /// Returns the position of the first non-occurrency of I
  ///
  /// Empty sequence
  template <int I>       // Element to search
  constexpr int firstNon<I> =0;
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns I element of the sequence
  ///
  /// Internal implementation
  template <int I,       // Position of element to search
	    int Head,    // First element
	    int...Tail>  // Other components
  constexpr int _getIntOfSeq()
  {
    if constexpr(I==0)
      return Head;
    else
      {
	static_assert(I<sizeof...(Tail)+1,"Asking for too large position");
	return _getIntOfSeq<I-1,Tail...>();
      }
  }
  
  /// Returns I element of the sequence
  template <int I,       // Position of element to search
	    int...Ints>  // Components
  [[ maybe_unused ]]
  constexpr int getIntOfSeq=_getIntOfSeq<I,Ints...>();
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Product of all integers
  template <int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hMul=Head*hMul<Tail...>;
  
  /// Product of all integers, unary case
  template <int Head>
  constexpr int hMul<Head> =Head;
  
  /////////////////////////////////////////////////////////////////////////
  
  DEFINE_BASE_TYPE(IntSeq);
  
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
  struct IntSeq :
    public BaseIntSeq
  {
    /// Length of the sequence of integer
    static constexpr int size=sizeof...(Ints);
    
    /// Sum of all elements
    static constexpr int hSum=SUNphi::hSum<Ints...>;
    
    /// Sum of all elements
    template <int I>
    static constexpr int hSumFirst=SUNphi::hSumFirst<I,Ints...>;
    
    /// Take first position wher I does not occur
    template <int I>
    static constexpr int firstNon=SUNphi::firstNon<I,Ints...>;
    
    /// Product of all elements
    static constexpr int hMul=SUNphi::hMul<Ints...>;
    
    /// Defines a new integer sequence incremented by a constant I
    template <int I>
    using Add=IntSeq<(Ints+I)...>;
    
    /// Defines a new integer sequence decreased by a constant I
    template <int I>
    using Sub=Add<-I>;
    
    /// Define a new integer sequence inflated by a constant factor I
    template <int I>
    using Mul=IntSeq<(Ints*I)...>;
    
    /// Define a new integer sequence decreased by a constant factor I
    template <int I>
    using Div=TypeIf<I!=0,Mul<1/I>>;
    
    /// Get the I element of the sequence
    template <int I>
    static constexpr int element=getIntOfSeq<I,Ints...>;
    
    /////////////////////////////////////////////////////////////////
    
    /// Append the first N id of another IntSeq
    ///
    /// Internal implementation, forward declaration
    template <bool,
	      int N,
	      typename Right>
    struct _AppendFirstN;
    
    /// Append the first N id of another IntSeq
    ///
    /// Internal implementation, taking recursively until N==0
    template <int N,
	      int HeadR,
	      int...TailR>
    struct _AppendFirstN<true,N,IntSeq<HeadR,TailR...>>
    {
      /// Returned type
      using type=typename IntSeq<Ints...,HeadR>::template AppendFirstN<N-1,IntSeq<TailR...>>;
    };
    
    /// Append the first N id of another IntSeq
    ///
    /// Internal implementation, terminator N==0
    template <int N,
	      int...R>
    struct _AppendFirstN<false,N,IntSeq<R...>>
    {
      /// Returned type
      using type=IntSeq<R...>;
    };
    
    /// Append the first N id of another IntSeq
    ///
    /// Gives visibility to internal implementation
    template <int N,
	      typename ISeq>
    using AppendFirstN=typename _AppendFirstN<(N>0),N,ISeq>::type;
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the first N components of an IntSeq
  ///
  /// Internal implementation
  template <int N,       // Number of id to take
	    typename ISeq, // IntSeq to operate upon
	    typename=ConstrainIsIntSeq<ISeq>>
  struct _IntSeqFirstN
  {
    /// Internal type
    using type=typename IntSeq<>::AppendFirstN<N,ISeq>;
  };
  
  /// Returns the first N components of an IntSeq
  ///
  /// Gives visibility to internal implementation
  template <int N,
	    typename ISeq,
	    typename=EnableIf<isIntSeq<ISeq>>>
  using FirstN=typename _IntSeqFirstN<N,ISeq>::type;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Internal implementation of sequence-of-integer catter.
  ///
  /// Dummy prototype to be specialized with actual case.
  template <class...T>
  struct _IntSeqCat;
  
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
  struct _IntSeqCat<IntSeq<Ints...>>
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
  template <int...Ints1,
	    int...Ints2,
	    class...T>
  struct _IntSeqCat<IntSeq<Ints1...>,IntSeq<Ints2...>,T...>
  {
    /// Binary cat \c Ints1..., \c Ints2..., separated for clarity
    using Nested=IntSeq<Ints1...,Ints2...>;
    
    /// Result of catting the whole list \c Ints1, \c Ints2,...
    typedef typename _IntSeqCat<Nested,T...>::type type;
  };
  
  /// Sequence-of-integer catter.
  ///
  /// Wraps the implementation to avoid writing "type"
  template <class...T>
  using IntSeqCat=typename _IntSeqCat<T...>::type;
  
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
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a IntSeq of size Num, all containing Entry as entry
  template <int Num,   // Number of components
	    int Val>   // Value to be used
  using IntSeqOfSameNumb=
    typename IntsUpTo<Num>::
    template Mul<0>::
    template Add<Val>;
  
  /////////////////////////////////////////////////////////////////
  
  
  /// Filter an IntSeq according to another one
  ///
  /// Internal implementation, forward declaration
  template <typename Is,
	    typename Fi>
  struct _FilterIntSeq;
  
  /// Filter an IntSeq according to another one
  ///
  /// Internal implementation
  template <int...Ints,
	    int...Ids>
  struct _FilterIntSeq<IntSeq<Ints...>,IntSeq<Ids...>>
  {
    /// Internal type
    using type=IntSeq<getIntOfSeq<Ids,Ints...>...>;
  };
  
  /// Filter an IntSeq according to another one
  template <typename Is,
	    typename Fi>
  using FilterIntSeq=typename _FilterIntSeq<Is,Fi>::type;
}

#endif
