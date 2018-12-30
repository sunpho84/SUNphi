#ifndef _ARITHMETIC_HPP
#define _ARITHMETIC_HPP

/// \file Arithmetic.hpp
///
/// \brief Defines several template version of simple arithmetic operations
///

#include <system/Debug.hpp>

namespace SUNphi
{
  /// Modulo operator, based on remainder operator %
  ///
  /// Valid on negative and positive numbers
  ///
  /// Example:
  ///
  /// \code
  /// safeModulo(5,3);  // 2
  /// safeModulo(-2,3); // 1
  /// safeModulo(-3,3); // 0
  /// \endcode
  ///
  template <typename T>
  T safeModulo(const T& val,   ///< Value of which to take the modulo
	       const T& mod)   ///< Modulo
  {
    ASM_BOOKMARK("BEGIN");
    
    /// Remainder
    const T r=
      val%mod;
    
    return
      (val<0 and r!=0)
      ?
      r+mod
      :
      r;
  }
}

#endif
