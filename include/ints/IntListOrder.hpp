#ifndef _INTLISTORDER_HPP
#define _INTLISTORDER_HPP

/// \file IntListOder.hpp
///
/// \brief Implements order check and operations on integer list

namespace SUNphi
{
  template <bool AlsoEqual,
	    int First,
	    int Second,
	    int...Tail>
  constexpr bool _areOrderedMin2Ints();
  
  template <bool AlsoEqual,
	    int...Ints>
  constexpr bool _areOrdered()
  {
    if constexpr(sizeof...(Ints)<2)
       return true;
    else
      return _areOrderedMin2Ints<AlsoEqual,Ints...>();
  }
  
  template <bool AlsoEqual,
	    int First,
	    int Second,
	    int...Tail>
  constexpr bool _areOrderedMin2()
  {
    constexpr bool compa=AlsoEqual?(First<=Second):(First<Second);
    
    return compa and _areOrdered<AlsoEqual,Second,Tail...>();
  }
  
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr bool areOrdered=
    _areOrdered<true,Ints...>();
  
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr bool areOrderedAndDifferent=
    _areOrdered<false,Ints...>();
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Returns the position of the first non-occurrency of I
  ///
  /// Internal implementaton
  template <int I,       // Element to search
	    int Pos,     // Position currently reached
	    int Head,    // First component to be searched
	    int...Ints>  // Other components
  constexpr int _firstNon()
  {
    if constexpr(I!=Head or sizeof...(Ints)==0)
      return Pos;
    else
      return _firstNon<I,Pos+1,Ints...>();
  }
  
  /// Returns the position of the first non-occurrency of I
  ///
  /// General case
  template <int I,       // Element to search
	    int...Ints>  // Components
  [[ maybe_unused ]]
  constexpr int firstNon=
    _firstNon<I,0,Ints...>();
  
  /// Returns the position of the first non-occurrency of I
  ///
  /// Empty sequence
  template <int I>       // Element to search
  constexpr int firstNon<I> =
    0;
}

#endif
