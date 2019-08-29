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
    
    /// Get the first or last combo
    Vector<Int> getFistOrLast(const bool& firstLast)
      const
    {
      /// Returned value
      Vector<Int> res(nSlots());
      
      /// Number of objects to assign
      Int nObjToAss=
	nObj;
      
      /// First slot to assign
      Slot firstSlot=
	(firstLast==true)
	?
	(nSlots()-1)
	:
	0;
      
      /// Last slot to assign
      Slot lastSlot=
	(firstLast==true)
	?
	-1
	:
	nSlots();
      
      /// Offset to move
      Slot dSlot=
	sign(lastSlot-firstSlot);
      
      // Move across all slots
      for(Slot iSlot=firstSlot;
	  iSlot!=lastSlot;
	  iSlot+=dSlot)
	{
	  // Assign to current slot
	  res[iSlot]=
	    std::min(nObjToAss,nMaxPerSlot[iSlot]);
	  
	  // Remove the assigned
	  nObjToAss-=
	    res[iSlot];
	}
      
      // Check that all have been assigned
      if(nObjToAss>0)
	CRASH<<"Should have ended without objects to assign, have: "<<nObjToAss;
      
      return
	res;
    }
    
  public:
    
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
      
      while(not found and iSlot<nSlots()-1)
	{
	  found=
	    canBeMovedRight(iSlot);
	  
	  if(not found)
	    iSlot++;
	}
      
      if(found)
	moveRight(iSlot);
      
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
      
      while(not found and iSlot<nSlots())
	{
	  found=
	    canBeMovedLeft(iSlot);
	  
	  if(not found)
	    iSlot++;
	}
      
      if(found)
	moveLeft(iSlot);
      
      return
	found;
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
	getFistOrLast(false);
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
	getFistOrLast(true);
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
		  const int nObj)                       ///< Number of objects
      : nMaxPerSlot(nMaxPerSlot),nObj(nObj)
    {
      // Check that the slots can accommodate the objects
      if(nMaxObj()<nObj)
	CRASH<<"Can accommodate at most "<<nMaxObj()<<" objects but "<<nObj<<" asked";
      
      // Set first
      setToFirst();
    }
  };
}

#endif
