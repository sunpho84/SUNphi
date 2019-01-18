#ifndef _COMBINATORIAL_HPP
#define _COMBINATORIAL_HPP

/// \file Combinatorial.hpp
///
/// \brief Header file for the definition of combinatorial routines
///
/// For the time being we limits ourselves to the combinatorial system

#include <cstdint>
#include <numeric>

#include <system/Vector.hpp>

namespace SUNphi
{
  /// Implements a looper over all combinations of a given kind
  template <typename Int=int64_t> // Integer type
  class Combinatorial
  {
    /// Type used for the size of vectors
    using Slot=
      typename Vector<Int>::Size;
    
    /// Maximal number of objects per slot
    Vector<Int> nMaxPerSlot;
    
    /// Number of objects
    Int nObj;
    
    /// Current combination
    Vector<Int> nPerSlot;
    
    /// Get the first or last combo
    Vector<Int> getFistOrLast(bool firstLast) const
    {
      /// Returned value
      Vector<Int> res(nSlots());
      
      /// Number of objects to assign
      Int nObjToAss=
	nObj;
      
      /// First slot to assign
      Slot firstSlot=
	(firstLast==false)?nSlots()-1:0;
      
      /// Last slot to assign
      Slot lastSlot=
	(firstLast==false)?-1:nSlots();
      
      /// Offset to move
      Slot dSlot=
	sign(lastSlot-firstSlot);
      
      // Moe across all slots
      for(Slot iSlot=firstSlot;iSlot!=lastSlot;iSlot+=dSlot)
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
	CRASH("Should have ended without objects to assign, have:",nObjToAss);
      
      return
	res;
    }
    
  public:
    
    /// Go to previous or next combo, returning true if was possible to do it
    bool rewindOrAdvance(const bool& rewindAdvance)
    {
      // Size 
      
      // // Scan the slots backward until the first non empty is found
      // Slot firstNonEmpty=
      // 	nSlots()-1;
    }     
    
    /// Get current combo
    const Vector<Int>& curCombo() const
    {
      return
	nPerSlot;
    }
    
    /// Get the firt combo
    Vector<Int> getFirst() const
    {
      return
	getFistOrLast(false);
    }
    
    /// Get the last combo
    Vector<Int> getLast() const
    {
      return
	getFistOrLast(true);
    }
    
    /// Number of slots
    Slot nSlots() const
    {
      return
	nMaxPerSlot.size();
    }
    
    /// Maximal number of objects that can be accommodated
    Int nMaxObj() const
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
	CRASH("Can accommodate at most",nMaxObj(),"objects but",nObj,"asked");
      
      // Set first
      nPerSlot=
	getFirst();
    }
    
    void setLast()
    {
    }
  };
}

#endif
