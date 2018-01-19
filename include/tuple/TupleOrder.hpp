#ifndef _TUPLEORDER_HPP
#define _TUPLEORDER_HPP

/// \file TupleOrder.hpp
///
/// \brief Defines filtering and reordering of Tuple

#include <ints/Ranges.hpp>
#include <tuple/TupleClass.hpp>

namespace SUNphi
{
  /// Gets the position of a type in a list
  ///
  /// Forward definition
  template <class T,
	    class...Tp>
  struct _PosOfType;
  
  /// Gets the position of a type in a list
  ///
  /// Case of matching type
  template <class T,
	    class...Tp>
  struct _PosOfType<T,T,Tp...>
  {
    static_assert(hSum<isSame<T,Tp>...> ==0,"Multiple occurrency of the searched type");
    
    /// Set the position to 0, the first of the list
    static constexpr int value=0;
  };
  
  /// Gets the position of a type in a list
  ///
  /// Case of non-matching type, instantiate iterativerly the searcher
  template <class T,
	    class U,
	    class...Tp>
  struct _PosOfType<T,U,Tp...>
  {
    static_assert(sizeof...(Tp),"Type not found in the list");
    
    /// Set the position to one more than the nested value
    static constexpr int value=1+_PosOfType<T,Tp...>::value;
  };
  
  /// Gets the position of a type in a tuple
  ///
  /// Wraps the ordinary list searcher
  template <class T,
	    class...Tp>
  struct _PosOfType<T,Tuple<Tp...>>
  {
    /// Position inside the list
    static constexpr int value=_PosOfType<T,Tp...>::value;
  };
  
  /// Gets the position of a type in a tuple or list
  ///
  /// Wraps the actual implementation
  ///
  /// \code
  /// int a=PosOfType<int,int,double,char>;        //0
  /// int b=PosOfType<int,Tuple<int,double,char>>; //0
  /// int c=PosOfType<int,char,double,char>;       //static_assert (not found)
  /// int d=PosOfType<int,int,int,char>;           //static_assert (multiple occurency)
  /// \endcode
  template <class...Args>
  [[ maybe_unused ]]
  constexpr int posOfType=_PosOfType<Args...>::value;
}

#endif
