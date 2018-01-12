#ifndef _UNARYPLUSMINUS_HPP
#define _UNARYPLUSMINUS_HPP

/// \file UnaryPlusMinus.hpp
///
/// \brief Header file for the definition of +tex or -tex

#include <metaprogramming/TypeTraits.hpp>
#include <tex/BaseTEx.hpp>

namespace SUNphi
{
  /// Implement +tex: decays into tex itself
  template <typename T,             // Type of the expression
	    SFINAE_ON_TEMPLATE_ARG(IsTEx<T>)>
  DECLAUTO operator+(T&& tex)       ///< Expression
  {
    return ~tex;
  }
}

#endif
