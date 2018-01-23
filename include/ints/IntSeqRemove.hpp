#ifndef _INTSEQREMOVE_HPP
#define _INTSEQREMOVE_HPP
#include <ints/IntListOrder.hpp>
#include <ints/IntSeq.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Remove the integer Ins in order of HeadAft,TailAft...
  ///
  /// Technically, the two IntSeq do not have to be ordered, the
  /// removal is done when HeadAft is largeer than Rem. The elements
  /// after are incremented by IncrAft. The removal is always accepted
  /// if IgnoreIfPresent is true.
  template <int Rem,                 // Element to remove
	    int IncrAft,             // Increment after the found position
	    bool IgnoreIfNotPresent, // Ignore removal if not present
	    int...Bef,               // Elements parsed so far
	    int HeadAft,             // Next element to parse
	    int...TailAft>           //Other elements to parse
  DECLAUTO _removeFromOrderedIntSeq(const IntSeq<Bef...>&,const IntSeq<HeadAft,TailAft...>&)
  {
    // Need to be removed now
    if constexpr(Rem<=HeadAft)
      // Present from the beginning
      if constexpr(Rem==HeadAft)
        return IntSeq<Bef...,(TailAft+IncrAft)...>{};
      else
	{
	  // If it was not present, check that we are fine with ignoring
	  static_assert(Rem!=HeadAft or IgnoreIfNotPresent,"Cannot ignore the absence");
	  return IntSeq<Bef...,HeadAft+IncrAft,(TailAft+IncrAft)...>{};
	}
    else
	// Move head to the left and nest
	if constexpr(sizeof...(TailAft)>0)
	   return _removeFromOrderedIntSeq<Rem,IncrAft,IgnoreIfNotPresent>(IntSeq<Bef...,HeadAft>{},IntSeq<TailAft...>{});
      else
	// Return the full
	return IntSeq<Bef...,HeadAft>{};
  }
  
  /// Remove the integer Ins in a previously empty IntSeq
  template <int Rem,
	    int IncrAft,
	    bool IgnoreIfPresent>
  DECLAUTO _removeFromOrderedIntSeq(const IntSeq<>&,const IntSeq<>&)
  {
    return IntSeq<>{};
  }
  
  /// Remove the integer Rem in an ordered IntSeq
  ///
  /// The elements after are incremented by IncrAft. The removal is
  /// valid if IgnoreIfNotPresent is true. Examples:
  ///
  /// \code
  /// using A=IntSeq<0,1,3>;
  /// using B=RemoveFromOrderedIntSeq<w,A,10,true>; // IntSeq<0,1,13>
  /// INVALID using C=InsertInOrderedIntSeq<3,A,10,false>; // IntSeq<0,1,3,13>
  /// \endcode
  template <int Ins,
	    typename IS,
	    int IncrAft=0,
	    bool IgnoreIfNotPresent=true>
  using RemoveFromOrderedIntSeq=decltype(_removeFromOrderedIntSeq<Ins,IncrAft,IgnoreIfNotPresent>(IntSeq<>{},typename ConstrainIsOrderedIntSeq<Unqualified<IS>>::type{}));
  
  /// Remove the integer Rem from an ordered unique IntSeq
  ///
  /// Care is taken to ensure that Ins is not already in IS The
  /// elements after are incremented by IncrAft. See
  /// RemovefromOrderedIntSeq for examples
  template <int Ins,
	    typename IS,
	    int IncrAft=0,
	    bool IgnoreIfPresent=false>
  using RemoveFromOrderedUniqueIntSeq=
    typename ConstrainIsOrderedUniqueIntSeq<
    decltype(_removeFromOrderedIntSeq<Ins,IncrAft,IgnoreIfPresent>
	      (IntSeq<>{},typename ConstrainIsOrderedUniqueIntSeq<IS>::type{}))>::type;
}

#endif
