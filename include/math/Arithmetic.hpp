#ifndef _ARITHMETIC_HPP
#define _ARITHMETIC_HPP

/// \file Arithmetic.hpp
///
/// \brief Defines several template version of simple arithmetic operations
///

#include <debug/OptimizationDiagnostic.hpp>

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
  
  /// Square of a number
  template <typename T>
  T sqr(const T& x)
  {
    return
      x*x;
  }
  
  /// Sign of a number, +1 0 or -1
  template <typename T>
  T sign(const T& x)
  {
    // Positive case
    if(x>0)
      return +1;
    
    // Negative case
    if(x<0)
      return -1;
    
    // Null case
    return
      0;
  }
  
  /// Return greatest common divisor between a and b
  template <typename I1,
	    typename I2>
  auto greatestCommonDivisor(const I1& _a,
			     const I2& _b)
  {
    /// Inner type to be used
    using I=
      decltype(_a/_b);
    
    /// Cast to I of \c _a
    I a=
      _a;
    
    /// Cast to I of \c _b
    I b=
      _b;
    
    while(a!=0)
      {
	/// Store temporarily a
	const I c=
	  a;
	
	a=
	  b%a;
	
	b=
	  c;
      }
    
    return
      b;
  }
}

#endif
