#ifndef _SIMD_HPP
#define _SIMD_HPP

/// \file SIMD.hpp
///
/// \brief Header file for Same Instruction Different Data instructions and types

#include <cstdlib>

#include <utility/Unused.hpp>

namespace SUNphi
{
  /// Basic alignement for AVX-512, to be generalized
  static constexpr size_t ALIGNMENT=64;
  MAYBE_UNUSED(ALIGNMENT);
  
  /// Number of components of a SIMD vector of type F
  template <typename F> // Fundamental type
  constexpr int NSIMD_COMPONENTS=
    ALIGNMENT/sizeof(F);
  
  /// Check if a certain number can be the size of a SIMD vector
  template <typename F>                                 //   Fundamental type
  constexpr bool canBeSizeOfSIMDVector(const int size)  ///< Size to be checked
  {
    return size>=0 and size%NSIMD_COMPONENTS<F> ==0;
  }
  
  /// Check if a certain number can be a factor of the size of a SIMD vector
  template <typename F>                                   //   Fundamental type
  constexpr bool canBeFactorOfSIMDVector(const int size)  ///< Size to be checked
  {
    return size>=0 and NSIMD_COMPONENTS<F>%size==0;
  }
}

#endif
