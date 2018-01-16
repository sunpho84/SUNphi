#ifndef _SWALLOWSEMICOLON_HPP
#define _SWALLOWSEMICOLON_HPP

/// \file SwallowSemicolon.hpp
///
/// \brief Macro to swallow a semicolon after a macro

namespace SUNphi
{
  
  /*! Eat a dangling semicolon inside a class
    
    Example:
    
    \code
    #define DEFINE_MEMBER_WITH_GETTER(TYPE,NAME) \
    TYPE NAME;					 \
    TYPE get_ ## NAME()				 \
    {						 \
    return NAME;				 \
    }						 \
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    class test_
    {
    DEFINE_MEMBER_WITH_GETTER(int,a);
    };
    \endcode
  */
#define SWALLOW_SEMICOLON_AT_CLASS_SCOPE	\
  class {}
  
  /*! Eat a dangling semicolon at global scope
    
    Example:
    
    \code
    #define DEFINE_FOOER(TYPE,NAME)			 \
    TYPE NAME()						 \
    {							 \
    doing something... 					 \
    }							 \
    SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
    
    DEFINE_FOER(int,ciccio);
    
    \endcode
  */
#define SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE			\
  /*! Forward definition of a never instantiated expression */	\
  void neverInstantiatedFunction()
}

#endif
