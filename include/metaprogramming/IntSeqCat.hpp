#ifndef _INTSEQCAT_HPP
#define _INTSEQCAT_HPP

/// \file IntSeqCat.hpp
///
/// \brief Concatenate IntSeq

#include <metaprogramming/IntSeq.hpp>

namespace SUNphi
{
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
