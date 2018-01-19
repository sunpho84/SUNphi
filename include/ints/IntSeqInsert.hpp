#ifndef _INTSEQINSERT_HPP
#define _INTSEQINSERT_HPP

#include <ints/IntListOrder.hpp>
#include <ints/IntSeq.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Insert the integer Ins in order of HeadAft,TailAft...
  ///
  /// Technically, the two IntSeq do not have to be ordered, the
  /// insertion is done when HeadAft is larger than Ins
  template <int Ins,
	    int...Bef,
	    int HeadAft,
	    int...TailAft>
  DECLAUTO _insertInOrderedIntSeq(const IntSeq<Bef...>&,const IntSeq<HeadAft,TailAft...>&)
  {
    // Need to be inserted now
    if constexpr(Ins<HeadAft)
		  return IntSeq<Bef...,Ins,HeadAft,TailAft...>{};
    else
      // Move head to the left and nest
      if constexpr(sizeof...(TailAft)>0)
		    return _insertInOrderedIntSeq<Ins>(IntSeq<Bef...,HeadAft>{},IntSeq<TailAft...>{});
      else
	// Put at the end
	return IntSeq<Bef...,HeadAft,Ins>{};
  }
  
  /// Insert the integer Ins in a previously empty IntSeq
  template <int Ins>
  DECLAUTO _insertInOrderedIntSeq(const IntSeq<>&,const IntSeq<>&)
  {
    return IntSeq<Ins>{};
  }
  
  /// Insert the integer Ins in an ordered IntSeq
  template <int Ins,
	    typename IS>
  using InsertInOrderedIntSeq=decltype(_insertInOrderedIntSeq<Ins>(IntSeq<>{},typename ConstrainIsOrderedIntSeq<IS>::type{}));
  
  /// Insert the integer Ins in an ordered unique IntSeq
  ///
  /// Care is taken to ensure that Ins is not already in IS
  template <int Ins,
	    typename IS>
  using InsertInOrderedUniqueIntSeq=
    typename ConstrainIsOrderedUniqueIntSeq<
     decltype(_insertInOrderedIntSeq<Ins>
	      (IntSeq<>{},typename ConstrainIsOrderedUniqueIntSeq<IS>::type{}))>::type;
}

#endif
