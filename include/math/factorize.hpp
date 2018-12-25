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
    // Simple case with 0: must crash
    if(n<=0)
      {
	CRASH("Unable to factorize %lld",(int64_t)n);
	return {};
      }
    
    // Simple case with 1 or 2: returns the number itself
    if(n<=2)
      {
	return {n};
      }
    
    /// Result of the factorization
    std::vector<T> out;
    
    /// Initial value of the divisor
    T d=2;
    
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
	  if(d==2)
	    d=3;
	  else
	    d+=2;
      }
    
    return out;
  }
}

#endif
