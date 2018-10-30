#ifndef _TUPLEORDER_HPP
#define _TUPLEORDER_HPP

/// \file TupleOrder.hpp
///
/// \brief Defines filtering and reordering of Tuple

#include <ints/Ranges.hpp>
#include <tuple/TupleClass.hpp>
#include <utility/Position.hpp>

namespace SUNphi
{
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
  constexpr int posOfTypeNotAsserting=
    _posOfType<DO_NOT_ASSERT_IF_NOT_PRESENT>(T{},Tp{});
  
  /////////////////////////////////////////////////////////////////
  
  /// Position of the types \c T in the Tuple \c Tp, asserting or not
  template <bool assertIfNotPresent,  // Flag determining whether has to be assert or return NOT_PRESENT
	    typename...T,             // Types to be searched
	    typename TP>              // Type of the \c Tuple where to search
  auto _posOfTypes(Tuple<T...> t,     ///< Holds the types to be searched
		   TP tp)             ///< \c Tuple where to search
    -> IntSeq<_posOfType<assertIfNotPresent>(T{},TP{})...>;
  
  /// Position of the Tuple types \c TpToSearch in the Tuple \c TpToProbe, asserting
  ///
  /// An expection is issued if the type is not founs
  template <typename TpToSearch, // Tuple containing the types to be searched
	    typename TpToProbe>  // Tuple where to search
  using posOfTypes=
    decltype(_posOfTypes<ASSERT_IF_NOT_PRESENT>(TpToSearch{},TpToProbe{}));
  
  /// Position of the Tuple types \c TpToSearch in the Tuple \c TpToProbe, not asserting
  ///
  /// NOT_PRESENT is returned if the type is not present
  template <typename TpToSearch, // Tuple containing the types to be searched
	    typename TpToProbe>  // Tuple where to search
  using posOfTypesNotAsserting=
    decltype(_posOfTypes<DO_NOT_ASSERT_IF_NOT_PRESENT>(TpToSearch{},TpToProbe{}));
}

#endif
