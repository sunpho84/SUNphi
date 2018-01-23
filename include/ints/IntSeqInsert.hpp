#ifndef _INTSEQINSERT_HPP
#define _INTSEQINSERT_HPP

#include <ints/IntListOrder.hpp>
#include <ints/IntSeq.hpp>
#include <ints/Ranges.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Insert the integer Ins in order of HeadAft,TailAft...
  ///
  /// Technically, the two IntSeq do not have to be ordered, the
  /// insertion is done when HeadAft is larger than Ins. The elements
  /// after are incremented by IncrAft. The insertion is ignored if
  /// IgnoreIfPresent is true.
  template <int Ins,              // Element to insert
	    int IncrAft,          // Increment after the found position
	    bool IgnoreIfPresent, // Ignore insertion if already present
	    int...Bef,            // Elements parsed so far
	    int HeadAft,          // Next element to parse
	    int...TailAft>        //Other elements to parse
  DECLAUTO _insertInOrderedIntSeq(const IntSeq<Bef...>&,const IntSeq<HeadAft,TailAft...>&)
  {
    // Need to be inserted now
    if constexpr(Ins<=HeadAft)
      // Returns original IntSeq
      if constexpr(Ins==HeadAft and IgnoreIfPresent)
	return IntSeq<Bef...,HeadAft,TailAft...>{};
      else
	// Insert and shift
	return IntSeq<Bef...,Ins,HeadAft+IncrAft,(TailAft+IncrAft)...>{};
    else
      // Move head to the left and nest
      if constexpr(sizeof...(TailAft)>0)
        return _insertInOrderedIntSeq<Ins,IncrAft,IgnoreIfPresent>(IntSeq<Bef...,HeadAft>{},IntSeq<TailAft...>{});
      else
	// Put at the end
	return IntSeq<Bef...,HeadAft,Ins>{};
  }
  
  /// Insert the integer Ins in a previously empty IntSeq
  template <int Ins,
	    int IncrAft,
	    bool IgnoreIfPresent>
  DECLAUTO _insertInOrderedIntSeq(const IntSeq<>&,const IntSeq<>&)
  {
    return IntSeq<Ins>{};
  }
  
  /// Insert the integer Ins in an ordered IntSeq
  ///
  /// The elements after are incremented by IncrAft. The insertion is
  /// ignored if IgnoreIfPresent is true. Examples:
  ///
  /// \code
  /// using A=IntSeq<0,1,3>;
  /// using B=InsertInOrderedIntSeq<3,A,10,true>; // IntSeq<0,1,3>
  /// using C=InsertInOrderedIntSeq<3,A,10,false>; // IntSeq<0,1,3,13>
  /// \endcode
  template <int Ins,
	    typename IS,
	    int IncrAft=0,
	    bool IgnoreIfPresent=false>
  using InsertInOrderedIntSeq=
    decltype(_insertInOrderedIntSeq<Ins,IncrAft,IgnoreIfPresent>(intSeq<>,typename ConstrainIsOrderedIntSeq<IS>::type{}));
  
  /////////////////////////////////////////////////////////////////
  
  /// Insert the elements of the IntSeq Is in an ordered IntSeq
  ///
  ///Internal implementation
  template <bool IgnoreIfPresent,
	    int HeadToIns,
	    int...TailToIns,
	    int HeadIncrAft,
	    int...TailIncrAft,
	    typename Is>
  DECLAUTO _InsertIntSeqInOrderedIntSeq(const IntSeq<HeadToIns,TailToIns...>& toIns,
					const IntSeq<HeadIncrAft,TailIncrAft...>& incrAft,
					const Is&)
  {
    // Insert HeadToIns
    InsertInOrderedIntSeq<HeadToIns,Is,HeadIncrAft,IgnoreIfPresent> tmp;
    
    // Returns HeadToIns insertion alone if no more present
    if constexpr(sizeof...(TailToIns)==0)
      return tmp;
    // Nest otherwise
    else
      return _InsertIntSeqInOrderedIntSeq<IgnoreIfPresent>(intSeq<TailToIns...>,intSeq<TailIncrAft...>,tmp);
  }
  
  /// Insert the elements of the IntSeq Is in an ordered IntSeq
  ///
  /// Internal implementation, empty case
  template <bool IgnoreIfPresent,
	    typename Is>
  DECLAUTO _InsertIntSeqInOrderedIntSeq(const IntSeq<>& noIns,const IntSeq<>& noIncrAft,const Is&)
  {
    return Is{};
  }
  
  /// Insert the elements of the IntSeq Is in an ordered IntSeq
  template <typename ToIns,                                     // IntSeq to be inserted
	    typename Is,                                        // IntSeq where to insert
	    typename IncrAft=IntSeqOfSameNumb<ToIns::size,0>,   // IntSeq of increments
	    bool IgnoreIfPresent=false,                         // If set true, ignore each already present entry
	    typename=EnableIf<isIntSeq<ToIns> and               // Checks that ToIns is an IntSeq
			      isIntSeq<IncrAft> and             // Checks that IncrAft is an IntSeq
			      ToIns::size==IncrAft::size>>      // Checks that ToIns and IncrAft have the same number of entries
  using InsertIntSeqInOrderedIntSeq=
    decltype(_InsertIntSeqInOrderedIntSeq<IgnoreIfPresent>(ToIns{},IncrAft{},Is{}));
}

#endif
