#ifndef _SWALLOWSEMICOLON_HPP
#define _SWALLOWSEMICOLON_HPP

/// \file SwallowSemicolon.hpp
///
/// \brief Macro to swallow a semicolon after a macro


/*! Define an anonymous empty class to eat a dangling semicolon
  
  Example:
 
 \code
 #define DEFINE_MEMBER_WITH_GETTER(TYPE,NAME)	 \
 TYPE NAME;					 \
 TYPE get_ ## NAME()				 \
 {						 \
    return NAME;				 \
 }						 \
 SWALLOW_SEMICOLON
 
 class test
 {
    DEFINE_MEMBER_WITH_GETTER(int,a);
 };
 \endcode
*/
#define SWALLOW_SEMICOLON			\
  class {}

#endif
