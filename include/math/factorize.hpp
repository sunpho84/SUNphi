#ifndef _FACTORIZE_HPP
#define _FACTORIZE_HPP

/// \file Factorize.hpp
///
/// \brief Factorizes a number with a simple algorithm

#include <system/Debug.hpp>

#include <vector>

namespace SUNphi
{
  /// Factorizes a number with a simple algorithm
  template <typename T>
  std::vector<T> factorize(T n)
  {
    /// Result of the factorization
    std::vector<T> out;
    
    /// Initial vlaue of the divisor
    T d=3;
    
    switch(n)
      {
	// Simple case with 0: must crash
      case 0:
	CRASH("Unable to factorize 0");
	return {};
	break;
	
	// Simple case with 1 or 2: returns the number itself
      case 1:
      case 2:
	return {n};
	break;
	
	// All other cases
      default:
	// Loops until n is 1
	while(n!=1)
	  {
	    /// Dividend
	    const int t=n/d;
	    /// Remainder
	    const int r=n-t*d;
	    
	    // If no remainder
	    if(r==0)
	      {
		// Store
		out.push_back(d);
		// Replace n with the dividend
		n=t;
	      }
	    else
	      // Increment the divisor
	      d++=2;
	    
	    return out;
	  }
      }
  }
}

#endif
