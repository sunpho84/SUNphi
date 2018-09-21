#ifndef _TUPLEORDER_HPP
#define _TUPLEORDER_HPP

/// \file TupleOrder.hpp
///
/// \brief Defines filtering and reordering of Tuple

#include <ints/Ranges.hpp>
#include <tuple/TupleClass.hpp>

namespace SUNphi
{
  /// Lament the absence of an element
  [[ maybe_unused ]]
  static constexpr bool
  ASSERT_IF_NOT_PRESENT=
    true;
  
  /// Do not lament the absence of an element
  [[ maybe_unused ]]
  static constexpr bool
  DO_NOT_ASSERT_IF_NOT_PRESENT=
    false;
  
  /// Mark the absence of an element
  static constexpr int NOT_PRESENT=-1;
  
  /// Gets the position of a type in a tuple
  ///
  /// Internal implementation, empty case
  template <bool assertIfNotPresent,
	    typename T>
  constexpr int _posOfType(T,Tuple<>)
  {
    // Return NOT_PRESENT or assert if empty
    static_assert(assertIfNotPresent==DO_NOT_ASSERT_IF_NOT_PRESENT,"Type not found in the list");
    
    return NOT_PRESENT;
  }
  
  /// Gets the position of a type in a tuple
  ///
  /// Internal implementation, matching case
  template <bool assertIfNotPresent,
	    typename T,
	    typename Head,
	    typename...Tail>
  constexpr int _posOfType(T,Tuple<Head,Tail...>)
  {
    /// Check if T is of the same type of Head
    constexpr bool isHead=
      isSame<T,Head>;
    
    /// Position in the nested list
    constexpr int posInTail=
      _posOfType<DO_NOT_ASSERT_IF_NOT_PRESENT>(T{},Tuple<Tail...>{});
    
    /// Mark if present in the Tail
    constexpr bool isInTail=
      posInTail!=NOT_PRESENT;
    
    // Matching case
    if constexpr(isHead)
      {
	static_assert(not isInTail,"Multiple case present!");
	
	return 0;
      }
    else
      if constexpr(isInTail)
	 return 1+posInTail;
      else
	return NOT_PRESENT;
  };
  
  /// Gets the position of a type in a tuple or list, asserting if not present
  ///
  /// Wraps the actual implementation
  ///
  /// \code
  /// int a=posOfType<int,int,double,char>;        //0
  /// int b=posOfType<int,Tuple<int,double,char>>; //0
  /// int c=posOfType<int,char,double,char>;       //static_assert (not found)
  /// int d=posOfType<int,int,int,char>;           //static_assert (multiple occurency)
  /// \endcode
  template <typename T,
	    typename Tp>
  [[ maybe_unused ]]
  constexpr int posOfType=
    _posOfType<ASSERT_IF_NOT_PRESENT>(T{},Tp{});
  
  /// Gets the position of a type in a tuple or list, not asserting if not present
  ///
  /// Wraps the actual implementation
  ///
  /// \code
  /// int a=posOfType<int,int,double,char>;        //0
  /// int b=posOfType<int,Tuple<int,double,char>>; //0
  /// int c=posOfType<int,char,double,char>;       //NOT_PRESENT
  /// int d=posOfType<int,int,int,char>;           //static_assert (multiple occurency)
  /// \endcode
  template <typename T,
	    typename Tp>
  [[ maybe_unused ]]
  constexpr int posOfTypeNotasserting=
    _posOfType<ASSERT_IF_NOT_PRESENT>(T{},Tp{});

}

#endif
