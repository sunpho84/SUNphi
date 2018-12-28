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
  template <typename T>      // Type of the variable
  bool bitOf(const T& in,   ///< Input variable
	     const int pos) ///< Bit to take
  {
    /// Size of the bitset
    constexpr int bitsetSize=
      sizeof(T);
    
    /// Bitset view of the variable
    using B=
      std::bitset<bitsetSize>;
    
    return
      static_cast<const B>(in)[pos];
  }
  
  /// Access to the lowest bit of a variable
  template <typename T>             // Type of the variable
  bool lowestBitOf(const T& in)   ///< Input variable
  {
    return bitOf(in,0);
  }
}

#endif
