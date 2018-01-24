#ifndef _UNARYPLUSMINUS_HPP
#define _UNARYPLUSMINUS_HPP

/// \file UnaryPlusMinus.hpp
///
/// \brief Header file for the definition of +smet or -smet

#include <metaprogramming/TypeTraits.hpp>
#include <smet/BaseSmET.hpp>

namespace SUNphi
{
  /// Implement +smet: decays into smet itself
  template <typename T,             // Type of the expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T>)>
  DECLAUTO operator+(T&& smet)       ///< Expression
  {
    return ~smet;
  }
}

#endif
