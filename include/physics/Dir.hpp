#ifndef _DIR_HPP
#define _DIR_HPP

/// \file Dir.hpp
///
/// \brief Header file for the inclusion of all dir related types

#include <tens/TensComp.hpp>
#include <smet/Bind.hpp>

namespace SUNphi
{
  // Defines Dir TensComp
  DEFINE_TENS_COMP(dir,Dir,DIR,4);
  /// \todo add number of dimension as an autoconf parameter
}

#endif
