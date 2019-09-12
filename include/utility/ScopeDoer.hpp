#ifndef _SCOPEDOER_HPP
#define _SCOPEDOER_HPP

/// \file ScopeDoer.hpp
///
/// \brief Scope-based action

#include <utility>

namespace SUNphi
{
  /// Class which does something when created and something else when destroyed
  ///
  /// The action performed at the beginning must return whether the
  /// action had an effect or not, such that the undo is issued or not
  /// at the destruction. The action performed at the end needs not to
  /// return anything (can return whatever).
  template <typename FEnd>   // Type of the function which is called at destruction
  class [[ nodiscard ]] ScopeDoer
  {
    /// Function to be called at destroy
    FEnd fEnd;
    
    /// Store whether we need to undo at the end
    bool undoAtEnd;
    
    /// Forbids copy constructor
    ScopeDoer(const ScopeDoer&) = delete;
    
  public:
    
    /// Check if will undo
    bool willUndo()
      const
    {
      return
	undoAtEnd;
    }
    
    /// Move constructor
    ScopeDoer(ScopeDoer&& oth) :
      fEnd(std::move(oth.fEnd)),
      undoAtEnd(oth.undoAtEnd)
      {
	oth.undoAtEnd=
	  false;
      }
    
    /// Create, do and set what to do at destruction
    template <typename FBegin> // Type of the function which is called at creation
    ScopeDoer(FBegin fBegin,
	      FEnd fEnd) :
      fEnd(fEnd)
    {
      undoAtEnd=
	fBegin();
    }
    
    /// Create, set what to do at destruction
    ScopeDoer(FEnd fEnd) :
      fEnd(fEnd)
    {
      undoAtEnd=
	true;
    }
    
    /// Destroy undoing
    ~ScopeDoer()
    {
      if(undoAtEnd)
	fEnd();
    }
  };
  
  /// Set a variable for the scope, change it back at the end
  template <typename T,
	    typename TV>
  auto getScopeChangeVar(T& ref,         ///< Reference
			 const TV& val)  ///< Value to set
  {
    /// Old value
    T oldVal=
      ref;
    
    ref=
      val;
    
    return
      ScopeDoer([&ref,oldVal]()
		{
		  ref=
		    oldVal;
		});
  }
}

#endif
