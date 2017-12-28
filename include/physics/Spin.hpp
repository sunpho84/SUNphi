#ifndef _SPIN_HPP
#define _SPIN_HPP

/// \file Spin.hpp
///
/// \brief Header file for the inclusion of all spin related types

#include <tens/TensComp.hpp>
#include <tex/Bind.hpp>

namespace SUNphi
{
  // Define Spin tens comp
  DEFINE_TENS_COMP(spin,Spin,NSPIN,4);
}

#endif
