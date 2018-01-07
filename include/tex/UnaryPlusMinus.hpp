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
  template <typename T>             // Type of the expression
  DECLAUTO operator+(TEx<T>&& tex)  ///< Expression
  {
    return ~tex;
  }
}

#endif
