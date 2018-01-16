#ifndef _PRINT_HPP
#define _PRINT_HPP

/// \file Print.hpp
///
/// \brief Header file to Print on a stream

#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Variadic print to a stream
  template <class S>
  S& print(S& out)
  {
    return out;
  }
  
  /// Variadic print to a stream
  template <class S,class Head,class...Tail>
  DECLAUTO print(S& out,const Head& head,Tail&&...tail)
  {
    out<<head;
    
    if constexpr(sizeof...(tail))
      {
	out<<" ";
	return print(out,forw<Tail>(tail)...);
      }
    else
      return out;
  }
}

#endif
