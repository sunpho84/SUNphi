#ifndef _INTLISTGETEL_HPP
#define _INTLISTGETEL_HPP

/// \file IntListGetEl.hpp
///
/// \brief Get an element of a list of integers

namespace SUNphi
{
  /// Returns I-th element of the integer list
  ///
  /// Internal implementation
  template <int I,       // Position of element to search
	    int Head,    // First element
	    int...Tail>  // Other components
  constexpr int _getIntOfList()
  {
    if constexpr(I==0)
      return Head;
    else
      {
	static_assert(I<sizeof...(Tail)+1,"Asking for too large position");
	return _getIntOfList<I-1,Tail...>();
      }
  }
  
  /// Returns I-th element of the sequence
  template <int I,       // Position of element to search
	    int...Ints>  // Components
  [[ maybe_unused ]]
  constexpr int getIntOfList=
    _getIntOfList<I,Ints...>();
}
#endif
