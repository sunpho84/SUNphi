#ifndef _COMPL_HPP
#define _COMPL_HPP

/// \file Compl.hpp
///
/// \brief Header file for the definition of complex numbers

#include <tens/TensComp.hpp>
#include <smet/Bind.hpp>

namespace SUNphi
{
  // Define complex number tens comp
  DEFINE_TENS_COMP(reIm,Compl,COMPL,2);
  
  /// Index of the real part of a \c Compl
  constexpr int REAL_PART_ID=
    0;
  
  /// Returns a reference to the real part
  template <typename T>     // Type of variable to be bound
  DECLAUTO real(T&& ref)    // Variable to be bound
  {
    return reIm(forw<T>(ref),REAL_PART_ID);
  }
  
  /// Index of the imag part of a \c Compl
  constexpr int IMAG_PART_ID=
    1;
  
  /// Returns a reference to the imag part
  template <typename T>     // Type of variable to be bound
  DECLAUTO imag(T&& ref)    // Variable to be bound
  {
    return reIm(forw<T>(ref),IMAG_PART_ID);
  }
}

#endif
