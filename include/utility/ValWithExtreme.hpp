#ifndef _VALWITHEXTREME_HPP
#define _VALWITHEXTREME_HPP

/// \file ValWithExtreme.hpp
///
/// \brief Defines a class which keeps track of extreme of a quantity
///
/// The class holds value in the internal variable \c val
/// The extreme value is host in the \c extr variable
/// When the class is implicitly access

#include <metaprogramming/SwallowSemicolon.hpp>
#include <ios/Logger.hpp>

namespace SUNphi
{
  /// Possible extreme types
  enum Extreme{MINIMUM,MAXIMUM};
  
  /// Class which keeps track of extreme values of a given type
  template <typename T,
	    Extreme E=MAXIMUM>
  class ValWithExtreme
  {
    /// Stored value
    T val;
    
    /// Extreme value
    T extr;
    
    /// Proxy holding value and extreme, needed to update extreme at destruction
    class ProxyVal
    {
      /// Reference to stored value
      T& val;
      
      /// Reference to extreme value
      T& extr;
      
    public:
      
      /// Constructor
      ProxyVal(T& val,
	       T& extr) :
	val(val),
	extr(extr)
      {
	SCOPE_INDENT(runLog);
      }
      
      /// Destructor
      ~ProxyVal()
      {
	/// Result of whether it's extreme or not
	bool is;
	
	switch(E)
	  {
	  case MINIMUM:
	    is=
	      (val<extr);
	    break;
	  case MAXIMUM:
	    is=
	      (val>extr);
	    break;
	  }
	
	if(is)
	  extr=
	    val;
      }
      
      /// Implicit cast to value
      operator T&()
      {
	return
	  val;
      }
    };
    
  public:
    
    /// Retrurn extreme value
    const T& extreme()
      const
    {
      return
	extr;
    }
    
    /// Constructor
    template <typename V=T>
    ValWithExtreme(const V& init=0) :
      val(init),
      extr(init)
    {
    }
    
    /// Implicit cast to const value reference
    operator const T&()
      const
    {
      return
    	val;
    }
    
    /// Implicit cast to proxy
    operator T&()
    {
      return
    	ProxyVal(val,extr);
    }
  };
}

#endif
