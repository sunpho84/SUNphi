#ifndef _BASETEX_HPP
#define _BASETEX_HPP

/// \file BaseTEx.hpp
///
/// \brief Header file defining basic properties of Template Expressions

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Defines the BaseTEx type traits
  DEFINE_BASE_TYPE(TEx);
  
  // Defines the check for a member "name"
  DEFINE_HAS_MEMBER(name);
  
  /// Defines the check for a TEx (empty so far)
#define STATIC_ASSERT_IS_TEX(...)
  
  /// TEmplate Expression
  template <typename T>
  struct TEx : public BaseTEx
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
}

#endif
