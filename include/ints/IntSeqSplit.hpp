#ifndef _SPLIT_HPP
#define _SPLIT_HPP

/// \file Split.hpp
///
/// \brief Take portions of integer sequences

#include <ints/IntSeq.hpp>

namespace SUNphi
{
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
  using IntSeqFirstN=typename _IntSeqFirstN<N,ISeq>::type;
  
  /////////////////////////////////////////////////////////////////
  
  
  /// Filter an IntSeq according to another one
  ///
  /// Internal implementation, forward declaration
  template <typename Is,
	    typename Fi>
  struct _IntSeqFilter;
  
  /// Filter an IntSeq according to another one
  ///
  /// Internal implementation
  template <int...Ints,
	    int...Ids>
  struct _IntSeqFilter<IntSeq<Ints...>,IntSeq<Ids...>>
  {
    /// Internal type
    using type=IntSeq<getIntOfList<Ids,Ints...>...>;
  };
  
  /// Filter an IntSeq according to another one
  template <typename Is,
	    typename Fi>
  using IntSeqFilter=typename _IntSeqFilter<Is,Fi>::type;
}

#endif
