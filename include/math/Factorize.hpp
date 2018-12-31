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
  std::vector<T> factorize(T n) ///< Number to factorize
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
    
    /// List of all known primes
    const std::vector<T> primesList=
      {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
    
    /// Iterator to the initial value of the divisor
    auto dPtr=
      primesList.begin();
    
    /// Divisor
    T d=
      *dPtr;
    
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
	  if(d<primesList.back())
	    d=
	      (*(++dPtr));
	  else
	    d+=
	      2;
      }
    
    return out;
  }
}

#endif
