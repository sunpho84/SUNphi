#ifndef _INTSEQGETEL_HPP
#define _INTSEQGETEL_HPP

#include <ints/IntSeqCat.hpp>
#include <utility/Position.hpp>

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
  /// \code
  ///
  /// int a=
  ///     posInIntSeqAfterAppending<2,4>(IntSeq<0,1,2,3){}>; // 2
  /// \endcode
  template <int Tail,                     // Tail to be added before extracting
	    bool ReturnNotPresent,        // Switch either returning \c NOT PRESENT or \c void, if not found
	    int Pos,                      // Position to extract
	    int...Head>                   // Integers from which to extract
  constexpr DECLAUTO _IntSeqGetElAfterAppending(IntSeq<Head...>)
  {
    /// Full \c IntSeq to be searched
    using Is=
      IntSeq<Head...,Tail>;
    
    /// Element search return
    constexpr int P=
      Is::template element<Pos>();
    
    // If the position is not present return empty, or \c NOT_PRESENT
    if constexpr(P==NOT_PRESENT and not ReturnNotPresent)
      return IntSeq<>{};
    else
      return IntSeq<P>{};
  }
  
  /// Modified extraction of an element in an \c IntSeq after appending a value
  ///
  /// Internal implementation
  template <int Tail,                     // Tail to be added before extracting
	    bool ReturnNotPresent,        // Switch either returning \c NOT PRESENT or \c void, if not found
	    int...Pos,                    // Positions to extract
	    int...Head>                   // Integers from which to extract
  DECLAUTO _IntSeqGetElsAfterAppending(IntSeq<Pos...>,
				      IntSeq<Head...>)
  {
    return IntSeqCat<decltype(_IntSeqGetElAfterAppending<Tail,ReturnNotPresent,Pos>(IntSeq<Head...>{}))...>{};
  }
  
  /// Modified extraction of an element in an \c IntSeq after appending a value
  ///
  /// Returns an \c IntSeq containing the element of all positions \c
  /// Pos in the passed \c IntSeq, after appending a given value to
  /// it. If asked, all the \c NOT_PRESENT are replaced with empty value.
  template <int Tail,                     // Tail to be added before extracting
	    bool ReturnNotPresent,        // Switch either returning \c NOT PRESENT or \c void, if not found
	    typename Pos,                 // \c IntSeq containing the positions to extract
	    typename Head>                // \c IntSeq containing the integers to extract
  using IntSeqGetElsAfterAppending=
    decltype(_IntSeqGetElsAfterAppending<Tail,ReturnNotPresent>(Pos{},Head{}));
}

#endif
