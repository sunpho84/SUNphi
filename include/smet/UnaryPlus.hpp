#ifndef _UNARYPLUS_HPP
#define _UNARYPLUS_HPP

/// \file UnaryPlus.hpp
///
/// \brief Header file for the definition of +smet

#include <metaprogramming/SFINAE.hpp>
#include <metaprogramming/UniversalReferences.hpp>
#include <smet/BaseSmET.hpp>

namespace SUNphi
{
  /// Implement +smet: decays into smet itself
  template <typename T,              // Type of the expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T>)>
  DECLAUTO operator+(T&& smet)       ///< Expression
  {
    return ~smet;
  }
}

#endif
