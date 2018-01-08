#ifndef _BASETEX_HPP
#define _BASETEX_HPP

/// \file BaseTEx.hpp
///
/// \brief Header file defining basic properties of Template Expressions

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/SwallowSemicolon.hpp>

namespace SUNphi
{
  /// Provides an attribute
#define ATTRIBUTE(DESCRIPTION,LONG_DESCRIPTION,TYPE,NAME,...)	\
  DESCRIPTION							\
  /*!             */						\
  LONG_DESCRIPTION						\
  static constexpr TYPE NAME=__VA_ARGS__
  
  /// Defines the BaseTEx type traits
  DEFINE_BASE_TYPE(TEx);
  
  // Defines the check for a member "name"
  DEFINE_HAS_MEMBER(name);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member "isStoring"
  DEFINE_HAS_MEMBER(isStoring);
  
  /// Provides a isStoring attribute
#define IS_STORING_ATTRIBUTE(LONG_DESCRIPTION,...)			\
  ATTRIBUTE(/*! Returns whether this TEx is storing */,LONG_DESCRIPTION,bool,isStoring,__VA_ARGS__)
  
  /// Set the TEx to storing
#define STORING						\
  IS_STORING_ATTRIBUTE(/*! This TEx is storing */,true)
  
  /// Set the TEx to not-storing
#define NOT_STORING						\
  IS_STORING_ATTRIBUTE(/*! This TEx is not storing */,false)
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member type "Tk"
  DEFINE_HAS_MEMBER(Tk);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member variable "isAliasing"
  DEFINE_HAS_MEMBER(isAliasing);
  
  /////////////////////////////////////////////////////////////////
  
  // Defines the check for a member variable "isAssignable"
  DEFINE_HAS_MEMBER(isAssignable);
  
  /// Provides a isAssignable attribute
#define IS_ASSIGNABLE_ATTRIBUTE(LONG_DESCRIPTION,...)			\
  ATTRIBUTE(/*! Returns whether this TEx can be the left hand of an assignement */,LONG_DESCRIPTION,bool,isAssignable,__VA_ARGS__)
  
  /// Set the TEx as assignable
#define ASSIGNABLE						\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx can be assigned */,true)
  
  /// Set the TEx as not-assignable
#define NOT_ASSIGNABLE						\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx cannot be assigned */,false)
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines the check for a TEx
  ///
  /// \todo add costRead
  /// \todo add costEval
  /// \todo add isAliasing
#define STATIC_ASSERT_IS_TEX(...)			\
  STATIC_ASSERT_HAS_MEMBER(isStoring,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(Tk,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(isAssignable,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(isAliasing,__VA_ARGS__)
  
  /// TEmplate Expression
  template <typename T>
  struct TEx :
    public BaseTEx
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
}

#endif
