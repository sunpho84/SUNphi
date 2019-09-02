#ifndef _FACTORIZE_HPP
#define _FACTORIZE_HPP

/// \file Factorize.hpp
///
/// \brief Factorizes a number with a simple algorithm

#include <ios/Logger.hpp>

#include <vector>

namespace SUNphi
{
  /// Factorizes a number with a simple algorithm
  template <typename I>
  constexpr Vector<I> factorize(I n) ///< Number to factorize
  {
    // Simple case with 0: must crash
    if(n<=0)
	return
	  {};
    
    // Simple case with 1 or 2: returns the number itself
    if(n<=2)
      return
	{n};
    
    /// Result of the factorization
    Vector<I> out;
    
    /// List of all known primes before 100
    constexpr std::array<I,25> primesList=
      {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
    
    /// Iterator to the initial value of the divisor
    auto dPtr=
      primesList.begin();
    
    /// Divisor
    I d=
      *dPtr;
    
    // Loops until n is 1
    while(n!=1)
      {
	/// Dividend
	const I t=
	  n/d;
	
	/// Remainder
	const I r=
	  n-t*d;
	
	// If no remainder
	if(r==0)
	  {
	    // Store
	    out.push_back(d);
	    // Replace n with the dividend
	    n=
	      t;
	  }
	else
	  // Increment the divisor
	  if(d<primesList.back())
	    d=
	      (*(++dPtr));
	  else
	    d+=
	      2;
      }
    
    return
      out;
  }
  
  /// Factorize grouping number
  ///
  /// Example:
  /// \code
  /// factorizeGrouping(12); // {{2,2},{3,1}}
  /// \endcode
  template <typename I>
  std::map<I,I> factorizeGrouping(const I& i)
  {
    /// Ungrouped factors
    const Vector<I> factors=
      factorize(i);
    
    return
      factors.group();
  }
}

#endif
