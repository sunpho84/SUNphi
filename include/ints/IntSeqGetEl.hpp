#ifndef _INTSEQGETEL_HPP
#define _INTSEQGETEL_HPP

#include <ints/IntSeqCat.hpp>

/// \file IntSeqGetEl.hpp
///
/// \brief Gets element from an \c IntSeq according to specific pattern

namespace SUNphi
{
  /// Modified extraction of an element in an \c IntSeq after appending a value
  ///
  /// Returns an \c IntSeq containing the element of position \c Pos
  /// in the passed \c IntSeq, after appending a given value to it. If
  /// asked, the returned \c NOT_PRESENT is replaced with empty value.
  ///
  /// \example
  ///
  /// int a=
  ///     posInIntSeqAfterAppending<2,4>(IntSeq<0,1,2,3){}>; // 2
  /// \endexample
  template <int Pos,                      // Integer to be searched
	    int Tail,                     // Tail to be added
	    bool returnNotPresent=true,   // Return NOT PRESENT, or void if not found
	    int...Head>                   // Integers already in the \c IntSeq
  constexpr DECLAUTO posInIntSeqAfterAppending(IntSeq<Head...>)
  {
    /// Full \c IntSeq to be searched
    using Is=
      IntSeq<Head...,Tail>;
    
    /// Element search return
    constexpr int P=
      Is::template element<Pos>();
    
    // If the position is not present return empty, or \c NOT_PRESENT
    if constexpr(P==NOT_PRESENT and not returnNotPresent)
      return IntSeq<>{};
    else
      return IntSeq<P>{};
  }
  
  /// Modified extraction of an element in an \c IntSeq after appending a value
  ///
  /// Returns an \c IntSeq containing the element of all positions \c
  /// Pos in the passed \c IntSeq, after appending a given value to
  /// it. If asked, all the \c NOT_PRESENT are replaced with empty value.
  template <int...Pos,                    // Integers to be searched
	    int Tail,                     // Tail to be added
	    bool returnNotPresent=true,   // Return NOT PRESENT, or void if not found
	    int...Head>                   // Integers already in the \c IntSeq
  DECLAUTO mergedDelimsInRef(IntSeq<Pos...>,
			     IntSeq<Head...>)
  {
    return IntSeqCat<decltype(posInIntSeqAfterAppending<Pos,Tail>(IntSeq<Head...>{}))...>{};
  }
}

#endif
