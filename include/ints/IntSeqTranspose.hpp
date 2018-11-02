#ifndef _INTSEQTRANSPOSE_HPP
#define _INTSEQTRANSPOSE_HPP

/// \file IntSeqTranspose.hpp
///
/// \brief Transpose a \c Tuple of \c IntSeq

#include <ints/IntSeq.hpp>
#include <ints/Ranges.hpp>
#include <tuple/TupleClass.hpp>

namespace SUNphi
{
  /// Slice a list of \c IntSeq according to a position
  ///
  /// Internal implementation performing checks and getting the \c
  /// IntSeq list out of the \c Tuple
  template<int Sl,           // Slice to get
	   typename...Is>    // List of IntSeq to slice
  DECLAUTO _IntSeqsSlice(Tuple<Is...> tupleOfIntSeqs)
  {
    static_assert(Sl>=0 and ((Sl<Is::size) && ...),"Sl must be between 0 and the maximal size of the IntSeq");
    
    return IntSeq<Is::template element<Sl>()...>{};
  }
  
  /// Slice a list of \c IntSeq according to a position
  ///
  /// Gives visibility to the internal implementation
  template<int Sl,           // Slice to get
	   typename Tp>      // \c Tuple of \c IntSeq to slice
  using IntSeqsSlice=
    decltype(_IntSeqsSlice<Sl>(Tp{}));
  
  /////////////////////////////////////////////////////////////////
  
  /// Slice a list of \c IntSeq according to a list of positions
  ///
  /// Internal implementation getting the list of slices out of the \c
  /// IntSeq, as well as the \c IntSeq out of the \c Tuple
  template<int...Sl,           // List of slices
	   typename...Is>      // List of \c IntSeq
  DECLAUTO _IntSeqsSlices(IntSeq<Sl...> listOfSlices,
			  Tuple<Is...> tupleOfIntSeqs)
  {
    /// Returned type
    using Ret=
      Tuple<IntSeqsSlice<Sl,Tuple<Is...>>...>;
    
    return Ret{};
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Transpose a list of \c IntSeq
  ///
  /// Internal implementation checking that all \c IntSeq have the
  /// same size and that the \c Tuple is not empty
  template<typename...Is>      // List of \c IntSeq
  DECLAUTO _IntSeqsTranspose(Tuple<Is...> tupleOfIntSeqs)
  {
    if constexpr(sizeof...(Is)!=0)
      {
	/// \c Tuple of \c IntSeq type
	using Tp=
	  Tuple<Is...>;
	
	/// Size of 0-th \c IntSeq
	constexpr int size0=
	  TupleElementType<0,Tp>::size;
	
	// Check that all \c IntSeq have the same size
	static_assert(((Is::size==size0) && ...),"All IntSeq must have ths same size");
	
	/// Returned type
	using Res=
	  decltype(_IntSeqsSlices(IntsUpTo<size0>{},
				  Tp{}));
	
	return Res{};
      }
    else
      return Tuple<>{};
  }
  
  /// Transpose a list of \c IntSeq
  ///
  /// Gives visibility to the internal implementation
  template<typename Tp>
  using IntSeqsTranspose=
    decltype(_IntSeqsTranspose(Tp{}));
}

#endif
