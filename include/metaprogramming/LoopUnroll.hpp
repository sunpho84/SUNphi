#ifndef _LOOP_UNROLL_HPP
#define _LOOP_UNROLL_HPP

#include <metaprogramming/Inline.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Unroll a loop in the range [min,max)
  ///
  /// The called function must take as a first argument the iteration
  template <int min,                               // Minimal value
	    int max,                               // Maximal (excluded) value
	    typename F,                            // Type of the function
	    typename...Args>                       // Types of the arguments to pass
  ALWAYS_INLINE void loopUnroll(F f,             ///< Function to cal
			       Args&&...args)    ///< Arguments to pass
  {
    /// Current value
    constexpr int cur=
      min;
    
    /// Next value
    constexpr int next=
      cur+1;
    
    // Exec the function
    f(cur,forw<Args>(args)...);
    
    // Iterates on the loop
    if constexpr(next<max)
		  loopUnroll<next,max,F,Args...>(f,forw<Args>(args)...);
  }
}

#endif
