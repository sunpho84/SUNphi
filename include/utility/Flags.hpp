#ifndef _FLAGS_HPP
#define _FLAGS_HPP

/// \file Flags.hpp
///
/// \brief Defines collectable flags
///
/// A flag is an integral value specifying the number of bits

#include <ints/IntSeq.hpp>

namespace SUNphi
{
  /// Create a mask for a given flag
  ///
  /// Shift right by the amount of bits
  template <auto Flag>
  [[ maybe_unused ]]
  constexpr int flagMask=
    1<<static_cast<int>(Flag);
  
  /// Add flags to a mask at compile time
  template <auto...Flags>   // Flags to be added
  [[ maybe_unused ]]
  constexpr int combineFlags=
       (flagMask<Flags> | ...);
  
  /// Add flags to a mask at compile time
  template <int InMask,     // Incoming flagset
	    auto...Flags>   // Flags to be added
  [[ maybe_unused ]]
  constexpr int addFlags=
       InMask | combineFlags<Flags...>;
  
  /// Remove flags to a mask at compile time
  template <int InMask,     // Incoming flagset
	    auto...Flags>   // Flags to be removed
  [[ maybe_unused ]]
  constexpr int remFlags=
       InMask ^ combineFlags<Flags...>;
  
  /// Get a given flag from a mask
  template <int Mask,
	    auto Flag>
  [[ maybe_unused ]]
  constexpr bool getFlag=
       Mask & flagMask<Flag>;
}

#endif
