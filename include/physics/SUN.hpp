#ifndef _SUN_HPP
#define _SUN_HPP

/// \file SUN.hpp
///
/// \brief Header file for the inclusion of all color types

#include <tens/TensComp.hpp>
#include <tex/Bind.hpp>

namespace SUNphi
{
  // Define Color tens comp
  DEFINE_TENS_COMP(col,Col,NCOL,3);
}

#endif
