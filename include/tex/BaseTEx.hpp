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
  /// Defines the BaseTEx type traits
  DEFINE_BASE_TYPE(TEx);
  
  // Defines the check for a member "name"
  DEFINE_HAS_MEMBER(name);
  
  // Defines the check for a member "isStoring"
  DEFINE_HAS_MEMBER(isStoring);
  
  // Defines the check for a member type "Tk"
  DEFINE_HAS_MEMBER(Tk);
  
  /// Defines the check for a TEx
#define STATIC_ASSERT_IS_TEX(...)			\
  STATIC_ASSERT_HAS_MEMBER(isStoring,__VA_ARGS__);	\
  STATIC_ASSERT_HAS_MEMBER(Tk,__VA_ARGS__)
  
  /// TEmplate Expression
  template <typename T>
  struct TEx : public BaseTEx
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Provides a simple evaluator (const and non-const)
#define PROVIDE_STRAIGHT_EVALUATOR(T)					\
  /*! Evaluator for type T */						\
  template <class...Args>						\
  friend decltype(auto) eval(T& EXP,const Args&...args)			\
  {									\
    return eval(EXP.ref,std::forward<const Args>(args)...);		\
  }									\
  									\
  /*! Evaluator for type T returning const*/				\
  template <class...Args>						\
  friend decltype(auto) eval(const T& EXP,const Args&...args)		\
  {									\
    return eval(EXP.ref,std::forward<const Args>(args)...);		\
  }									\
  SWALLOW_SEMICOLON
}

#endif
