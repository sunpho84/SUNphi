#ifndef _INTLISTORDER_HPP
#define _INTLISTORDER_HPP

/// \file IntListOrder.hpp
///
/// \brief Implements order check and operations on integer list

namespace SUNphi
{
  /// Compare the order of at least two integers
  ///
  /// Return true if First<Second or First==Second (only if
  /// AlsoEqual==true). Call iteratively itself if more than two
  /// arguents are passed.
  template <bool AlsoEqual, // Flag to return true if First==Second
	    int First,      // First element to compare
	    int Second,     // Second element to compare
	    int...Tail>     // Other elements to compare
  constexpr bool _areOrderedMin2Ints();
  
  /// Compare a list of integer
  ///
  /// If less than two elements are pased, returns true. Otherwise
  /// call the routine which operates on at least two integers
  template <bool AlsoEqual, // Flag to return true if First==Second
	    int...Ints>     // List of integers to compare
  constexpr bool _areOrdered()
  {
    if constexpr(sizeof...(Ints)<2)
       return true;
    else
      return _areOrderedMin2Ints<AlsoEqual,Ints...>();
  }
  
  // Actual implementation (see above for comments)
  template <bool AlsoEqual,
	    int First,
	    int Second,
	    int...Tail>
  constexpr bool _areOrderedMin2Ints()
  {
    constexpr bool compa=AlsoEqual?(First<=Second):(First<Second);
    
    return compa and _areOrdered<AlsoEqual,Second,Tail...>();
  }
  
  /// Returns true if all elements are ascending-ordered and different
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr bool areOrdered=
    _areOrdered<true,Ints...>();
  
  /// Returns true if all elements are weak ascending-ordered
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
