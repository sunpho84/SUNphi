#ifndef _INT_LIST_OPERATIONS_HPP
#define _INT_LIST_OPERATIONS_HPP

/// \file IntListOperations.hpp
///
/// \brief Implements operations over list of integers

namespace SUNphi
{
  /// Sum of all integers
  template <int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hSum=
    Head+hSum<Tail...>;
  
  /// Sum of all integers, unary case
  template <int Head>
  constexpr int hSum<Head> =
    Head;
  
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
  template <int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hMul=
    Head*hMul<Tail...>;
  
  /// Product of all integers, unary case
  template <int Head>
  constexpr int hMul<Head> =
    Head;
}

#endif
