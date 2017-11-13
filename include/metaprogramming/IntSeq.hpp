#ifndef _INTSEQ_HPP
#define _INTSEQ_HPP

/// \file IntSeq.hpp
///
/// \brief Implements a struct holding a sequence of integers.

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
  using IntSeqCatT=typename IntSeqCatImpl<T...>::type;
}

#endif
