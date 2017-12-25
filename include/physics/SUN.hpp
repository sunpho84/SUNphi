#ifndef _SUN_HPP
#define _SUN_HPP

#include <tens/TensComp.hpp>
#include <tex/Bind.hpp>

namespace SUNphi
{
  // Define Compl tens comp
  DEFINE_TENS_COMP(reim,Compl,NCOMPL,2);
  
  // Define Color tens comp
  DEFINE_TENS_COMP(col,Col,NCOL,3);
  
  // Define Spin tens comp
  DEFINE_TENS_COMP(spin,Spin,NSPIN,4);
  
  // Define Space time tens comp
  DEFINE_TENS_COMP(site,Spacetime,DUMMY_VOLUME,DYNAMIC);
  
  /// Index of the real part of a \c Compl
  constexpr int REAL_PART_ID=0;
  
  /// Returns a reference to the real part
  template <typename T> // Type of variable to be bound
  auto real(T&& ref)    // Variable to be bound
  {
    return reim(ref,REAL_PART_ID);
  }
  
  /// Index of the imag part of a \c Compl
  constexpr int IMAG_PART_ID=1;
  
  /// Returns a reference to the imag part
  template <typename T> // Type of variable to be bound
  auto imag(T&& ref)    // Variable to be bound
  {
    return reim(ref,IMAG_PART_ID);
  }
}

#endif
