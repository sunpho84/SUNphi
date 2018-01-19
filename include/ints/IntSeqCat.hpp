#ifndef _INTSEQCAT_HPP
#define _INTSEQCAT_HPP

/// \file IntSeqCat.hpp
///
/// \brief Cat one or more integer sequences and lists

#include <ints/IntSeq.hpp>

namespace SUNphi
{
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
}

#endif
