#ifndef _INT_LIST_OPERATIONS_HPP
#define _INT_LIST_OPERATIONS_HPP

/// \file IntListOperations.hpp
///
/// \brief Implements operations over list of integers

namespace SUNphi
{
  /// Sum of all integers
  ///
  /// Internal implementation
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr int _hSum()
  {
    if constexpr(sizeof...(Ints)==0)
      return 0;
    else
      return (Ints + ...);
  }
  
  /// Sum of all integers
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr int hSum=_hSum<Ints...>();
  
  /////////////////////////////////////////////////////////////////
  
  /// Sum of all integers up to I (excluded)
  template <int I,
	    int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hSumFirst=
    (I>0)*Head+hSumFirst<I-1,Tail...>;
  
  /// Sum of all integers up to I (excluded),unary case
  template <int I,
	    int Head>
  constexpr int hSumFirst<I,Head> =
    (I>0)*Head;
  
  /////////////////////////////////////////////////////////////////
  
  /// Product of all integers
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr int hMul=(Ints * ...);
}

#endif
