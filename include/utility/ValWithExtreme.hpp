#ifndef _VALWITHEXTREME_HPP
#define _VALWITHEXTREME_HPP

/// \file ValWithExtreme.hpp
///
/// \brief Defines a class which keeps track of extreme of a quantity
///
/// The class holds value in the internal variable \c val
/// The extreme value is host in the \c extr variable
/// When the class is implicitly access

#include <limits>

#include <metaprogramming/SwallowSemicolon.hpp>

namespace SUNphi
{
  /// Possible extreme types
  enum Extreme{MINIMUM,MAXIMUM};
  
  /// Class which keeps track of extreme values of a given type
  template <typename T,
	    Extreme E>
  class ValWithExtreme
  {
    /// Stored value
    T val;
    
    /// Extreme value
    T extr;
    
    /// Update the extreme
    ValWithExtreme& updateExtreme()
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
      
      return
	*this;
    }
    
  public:
    
    /// Retrurn extreme value
    const T& extreme()
      const
    {
      return
	extr;
    }
    
    /// Reset to standard value
    void reset()
    {
      switch(E)
	{
	case MINIMUM:
	  extr=
	    std::numeric_limits<T>::max();
	  break;
	case MAXIMUM:
	  extr=
	    std::numeric_limits<T>::min();
	  break;
	}
    }
    
    /// Constructor
    template <typename V=T>
    ValWithExtreme(const V& init) :
      val(init),
      extr(init)
    {
    }
    
    /// Default constructor
    ValWithExtreme()
    {
      reset();
    }
    
    /// Implicit cast to const value reference
    operator const T&()
      const
    {
      return
    	val;
    }
    
    /// Provide an unary operator \c OP
#define PROVIDE_UNARY_OPERATOR(OP)		\
    /*! Unary operator \c OP with update */	\
    template <typename V>			\
    ValWithExtreme& operator OP (const V& oth)	\
    {						\
    val OP					\
    oth;					\
						\
    return					\
      updateExtreme();				\
    }						\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_UNARY_OPERATOR(+=);
    
    PROVIDE_UNARY_OPERATOR(-=);
    
    PROVIDE_UNARY_OPERATOR(=);
    
#undef PROVIDE_UNARY_OPERATOR
  };
  
  /// class to keep a value and its maximum
  template <typename T>
  using ValWithMax=
    ValWithExtreme<T,MAXIMUM>;
}

#endif
