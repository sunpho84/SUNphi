#ifndef _SCOPEDOER_HPP
#define _SCOPEDOER_HPP

/// \file ScopeDoer.hpp
///
/// \brief Scope-based action

namespace SUNphi
{
  /// Class which does something when created and something else when destroyed
  ///
  /// The action performed at the beginning must return whether the
  /// action had an effect or not, such that the undo is issued or not
  /// at the destruction. The action performed at the end needs not to
  /// return anything (can return whatever).
  template <typename FBegin, // Type of the function which is called at creation
	    typename FEnd>   // Type of the function which is called at destruction
  class ScopeDoer
  {
    /// Function to be called at destroy
    FEnd fEnd;
    
    /// Store whether we need to undo at the end
    bool undoAtEnd;
    
  public:
    
    /// Create, do and set what to do at undo
    ScopeDoer(FBegin fBegin,
	      FEnd fEnd) :
      fEnd(fEnd)
    {
      undoAtEnd=
	fBegin();
    }
    
    /// Destroy undoing
    ~ScopeDoer()
    {
      if(undoAtEnd)
	fEnd();
    }
  };
  
  /// Deduction guide for ScopeDoer
  template <typename FBegin,
	    typename FEnd>
  ScopeDoer(FBegin fBegin,
	    FEnd fEnd)
    -> ScopeDoer<FBegin,FEnd>;
  
  
  /// Change the variable for the object scope
  template <typename T>
  class ScopeChangeVar
  {
    /// Reference
    T& ref;
    
    /// Old value
    const T oldVal;
    
  public:
    
    /// Create and increase indent level
    ScopeChangeVar(T& ref,        ///< Reference to change
		   const T& val)  ///< Value to set
      : ref(ref),oldVal(ref)
    {
      // Set the new value
      ref=
  	val;
    }
    
    /// Delete and decrease indent level
    ~ScopeChangeVar()
    {
      // Set back the old value
      ref=
  	oldVal;
    }
  };
  
  /// Deduction guide for ScopeChangeVar
  template <typename T>
  ScopeChangeVar(T& ref,
		 const T& val)
    -> ScopeChangeVar<T>;
}

#endif
