#ifndef _PRINT_HPP
#define _PRINT_HPP

/// \file Print.hpp
///
/// \brief Header file to Print on a stream

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
  S& print(S& out,const Head& head,const Tail&...tail)
  {
    out<<head;
    if(sizeof...(tail)) out<<" ";
    
    return print(out,tail...);
  }
}

#endif
