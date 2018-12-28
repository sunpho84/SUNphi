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
  bool bitOf(const W& w,const int pos=0)
  {
    /// Size of the bitset
    constexpr int bitsetSize=
      sizeof(W);
    
    /// Bitset view of the variable
    using B=
      std::bitset<bitsetSize>;
    
    return
      static_cast<const B>(w)[pos];
  }
}

#endif
