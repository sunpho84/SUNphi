#ifndef _SUN_HPP
#define _SUN_HPP

/// \file SUN.hpp
///
/// \brief Header file for the inclusion of all color types

#include <tens/TensComp.hpp>
#include <tens/TwinsComp.hpp>
#include <smet/Bind.hpp>

namespace SUNphi
{
  // Define Col TensComp
  DEFINE_RW_CN_TENS_COMP(col,Col,COL,3);
}

#endif
