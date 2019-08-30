#ifndef _COMBINATORIAL_HPP
#define _COMBINATORIAL_HPP

/// \file Combinatorial.hpp
///
/// \brief Header file for the definition of combinatorial routines
///
/// For the time being we limits ourselves to the combinatorial system

#include <cstdint>
#include <numeric>

#include <containers/Vector.hpp>
#include <debug/Crash.hpp>
#include <ios/Logger.hpp>

namespace SUNphi
{
  /// Implements a looper over all combinations of a given kind
  template <typename Int=int64_t> // Integer type
  class Combinatorial
  {
    /// Type used to enumerate the slots
    using Slot=
      typename Vector<Int>::Size;
    
    /// Maximal number of objects per slot
    Vector<Int> nMaxPerSlot;
    
    /// Number of objects
    Int nObj;
    
    /// Current combination
    Vector<Int> nPerSlot;
    
    /// Assign n elements n the range, putting the maximum available in each of them
    ///
    /// Returns the number of non-assigned elements
    Int assignToSlots(Vector<Int>& res,          ///< Result
		      Int nObjToAss,             ///< Number of elements to assign
		      const Slot& firstSlot,     ///< First slot where to assign
		      const Slot& pastLastSlot, ///< Slot before which to stop assigning
		      const Slot& dSlot)          ///< Offset between slots
      const
    {
      // Move across all slots
      for(Slot iSlot=firstSlot;
	  iSlot!=pastLastSlot;
	  iSlot+=dSlot)
	{
	  // Assign to current slot
	  res[iSlot]=
	    std::min(nObjToAss,nMaxPerSlot[iSlot]);
	  
	  // Remove the assigned
	  nObjToAss-=
	    res[iSlot];
	}
      
      return
	nObjToAss;
    }
    
    /// Get the first or last combo
    Vector<Int> getFirstOrLast(const FIRST_OR_LAST& firstLast)
      const
    {
      /// Returned value
      Vector<Int> res(nSlots());
      
      /// Number of objects to assign
      const Int nObjToAss=
	nObj;
      
      /// First slot to assign
      const Slot firstSlot=
	(firstLast==LAST)
	?
	(nSlots()-1)
	:
	0;
      
      /// Last slot to assign
      const Slot pastLastSlot=
	(firstLast==LAST)
	?
	-1
	:
	(nSlots());
      
      /// Offset to move
      const Slot dSlot=
	sign(pastLastSlot-firstSlot);
      
      /// Residual objects to assign
      const Int nResObjToAss=
	assignToSlots(res,nObjToAss,firstSlot,pastLastSlot,dSlot);
      
      // Check that all have been assigned
      if(nResObjToAss>0)
	CRASH<<"Should have ended without objects to assign, have: "<<nResObjToAss;
      
      return
	res;
    }
    
  public:
    
    /// Check if it is possible to combine the nObj into the slots
    static bool isPossibleToAccomodate(const Vector<Int>& nMaxPerSlot,       ///< Maximal number of objects per slot
				       const int nObj)                       ///< Number of objects
    {
      return
	nMaxPerSlot.summatorial()>=nObj;
    }
    
    /// Check if the slot is free
    bool isSlotFree(const Slot& iSlot)
      const
    {
      return
	nPerSlot[iSlot]==0;
    }
    
    /// Check if the slot is fully occupied
    bool isSlotFullyOccupied(const Slot& iSlot)
      const
    {
      return
	nPerSlot[iSlot]==nMaxPerSlot[iSlot];
    }
    
    /// Check if 1 object can be moved from the slot to the right one
    bool canBeMovedRight(const Slot& iSlot)
      const
    {
      return
	(not (isSlotFullyOccupied(iSlot+1) or isSlotFree(iSlot)));
    }
    
    /// Move right one element from the slot
    void moveRight(const Slot& iSlot)
    {
      nPerSlot[iSlot]--;
      nPerSlot[iSlot+1]++;
    }
    
    /// Check if 1 object can be moved from the slot to the left one
    bool canBeMovedLeft(const Slot& iSlot)
      const
    {
      return
	(not (isSlotFullyOccupied(iSlot-1) or isSlotFree(iSlot)));
    }
    
    /// Move left one element from the slot
    void moveLeft(const Slot& iSlot)
    {
      nPerSlot[iSlot-1]++;
      nPerSlot[iSlot]--;
    }
    
    /// Go to next combo, returning true if was possible to do it
    bool advance()
    {
      /// Slot to move
      Slot iSlot=
	0;
      
      /// Returned value
      bool found=
	false;
      
      /// Number of object up to the slot
      Int nUpToSlot=
	0;
      
      while(not found and iSlot<nSlots()-1)
	{
	  found=
	    canBeMovedRight(iSlot);
	  
	  nUpToSlot+=
	    nPerSlot[iSlot];
	  
	  if(not found)
	    iSlot++;
	}
      
      if(found)
	{
	  nPerSlot[iSlot+1]++;
	  assignToSlots(nPerSlot,nUpToSlot-1,0,iSlot+1,+1);
	}
      
      return
	found;
    }
    
    /// Go to previous combo, returning true if was possible to do it
    bool rewind()
    {
      /// Slot to move
      Slot iSlot=
	1;
      
      /// Returned value
      bool found=
	false;
      
      /// Number of object before the slot
      Int nUpToPreviousSlot=
	0;
      
      while(not found and iSlot<nSlots())
	{
	  found=
	    canBeMovedLeft(iSlot);
	  
	  nUpToPreviousSlot+=
	    nPerSlot[iSlot-1];
	  
	  if(not found)
	    iSlot++;
	}
      
      if(found)
	{
	  nPerSlot[iSlot]--;
	  assignToSlots(nPerSlot,nUpToPreviousSlot+1,0,iSlot,+1);
	}
      
      return
	found;
    }
    
    /// Go backward or advance depending on the passed parameter
    bool rewindOrAdvance(const BACK_FORW& BackForw)
    {
      if(BackForw==BACK)
	return
	  rewind();
      else
	return
	  advance();
    }
    
    /// Const cast to the combinatorial
    const Vector<Int>& operator()()
      const
    {
      return
	nPerSlot;
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(operator());
    
    /// Get the firt combo
    Vector<Int> getFirst()
      const
    {
      return
	getFirstOrLast(FIRST);
    }
    
    /// Set to first combo
    void setToFirst()
    {
      nPerSlot=
	getFirst();
    }
    
    /// Set to last combo
    void setToLast()
    {
      nPerSlot=
	getLast();
    }
    
    /// Get the last combo
    Vector<Int> getLast()
      const
    {
      return
	getFirstOrLast(LAST);
    }
    
    /// Number of slots
    Slot nSlots()
      const
    {
      return
	nMaxPerSlot.size();
    }
    
    /// Maximal number of objects that can be accommodated
    Int nMaxObj()
      const
    {
      return
	nMaxPerSlot.summatorial();
    }
    
    /// Constructor specifying the maximal number of objects per slot
    Combinatorial(const Vector<Int>& nMaxPerSlot,       ///< Maximal number of objects per slot
		  const int nObj,                       ///< Number of objects
		  const bool lastOrFirst=false)         ///< Starts from first or last combo
      : nMaxPerSlot(nMaxPerSlot),nObj(nObj)
    {
      // Check that the slots can accommodate the objects
      if(not isPossibleToAccomodate(nMaxPerSlot,nObj))
	 CRASH<<"Can accommodate at most "<<nMaxObj()<<" objects but "<<nObj<<" asked";
      
      // Set first
      if(lastOrFirst==false)
	setToFirst();
      else
	setToLast();
    }
  };
  
  /// Loop on all combinations
  ///
  /// Run the provided function, passing the combo at each iteration,
  /// and returning the total number of combinations. If BackForw is
  /// true, loop forward, otherwise loop backward
  template <typename Int,
	    typename Fun>
  Int loopOnAllCombinations(const Vector<Int>& nMaxPerSlot,       ///< Maximal number of objects per slot
			    const int nObj,                       ///< Number of objects
			    const Fun& fun,                       ///< Function to be called
			    BACK_FORW BackForw=FORW)              ///< Loop direction
  {
    /// Count
    Int n=
      0;
    
    if(Combinatorial<Int>::isPossibleToAccomodate(nMaxPerSlot,nObj))
      {
	
	
	/// Combination generator
	Combinatorial<Int> combo(nMaxPerSlot,nObj,
				 ((BackForw==FORW)?
				  FIRST:
				  LAST));
	
	if(BackForw==BACK)
	  combo.setToLast();
	// else ... not needed, combo is automaticall
	
	do
	  {
	    fun(combo());
	    n++;
	  }
	while(combo.rewindOrAdvance(BackForw));
      }
    
    return
      n;
  }
}

#endif
