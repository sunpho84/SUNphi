#ifndef _SPIN_HPP
#define _SPIN_HPP

/// \file Spin.hpp
///
/// \brief Header file for the inclusion of all spin related types

#include <tens/TensComp.hpp>
#include <tens/TwinsComp.hpp>
#include <tex/Bind.hpp>

namespace SUNphi
{
  // Defines Spin TensComp
  DEFINE_RW_CN_TENS_COMP(spin,Spin,SPIN,4);
}

#endif
