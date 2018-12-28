#ifndef _BITS_HPP
#define _BITS_HPP

/// \file Bits.hpp
///
/// \brief Access a variable by bit
///

#include <bitset>

#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Access to a given bit of a variable
  template <typename W>
  DECLAUTO getBit(W&& w,const int pos)
  {
    /// Bitset view of the variable
    using B=
      std::bitset<sizeof(W)>;
    
    return static_cast<B>(w)[pos];
  }
}

#endif
