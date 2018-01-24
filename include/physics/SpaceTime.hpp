#ifndef _SPACETIME_HPP
#define _SPACETIME_HPP

/// \file SpaceTime.hpp
///
/// \brief Header file for the inclusion of all Spacetime types

#include <tens/TensComp.hpp>
#include <smet/Bind.hpp>

namespace SUNphi
{
  // Define Spacetime tens comp
  DEFINE_TENS_COMP(site,Spacetime,DUMMY_VOLUME,DYNAMIC);
}

#endif
