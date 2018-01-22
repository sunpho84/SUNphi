#ifndef _TUPLEELEMENTS_HPP
#define _TUPLEELEMENTS_HPP

/// \file TupleElements.hpp
///
/// \brief Access elements of a tuple

#include <ints/Ranges.hpp>
#include <metaprogramming/UniversalReferences.hpp>
#include <tuple/TupleClass.hpp>
#include <tuple/TupleOrder.hpp>

namespace SUNphi
{
  /// Gets an element from the tuple.
  ///
  /// Directly aliasing the std library. R-value case
  /// \return A move-reference to the I tuple element.
  template <std::size_t I,
	    class T>
  constexpr std::tuple_element_t<I,T>&&
  get(T&& t)
  {
    return std::get<I>(forw<T>(t));
  }
  
  /// Gets the element of type T from the tuple.
  ///
  /// Directly aliasing the std library. R-value case.
  /// \return A move-reference to the T tuple type (if present).
  template <class Tg,
	    class T>
  constexpr Tg&& get(T&& t)
  {
    return std::get<Tg>(forw<T>(t));
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Loop over all \c Tuple elements
  ///
  /// Non const access to all elements of the \c Tuple, called
  /// recursively until I==sizeof...(Tp), incrementing the parameter
  /// I.
  ///
  /// \tparam I Index of the \c Tuple element to extract
  /// \tparam Func \c Function type
  /// \tparam Tp... The \c Tuple parameters
  /// \return \c void
  template <size_t I=0,
	    typename T,
	    typename F,
	    typename=EnableIf<isTuple<T>>>
  void forEach(T&& t, ///< \c Tuple to act upon
	       F&& f) ///< \c Function iterating on the \c Tuple
  {
    if constexpr(I<tupleSize<T>)
       {
	 f(get<I>(forw<T>(t)));
	 forEach<I+1>(forw<T>(t),forw<F>(f));
       }
  }
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Filter a \c Tuple on the basis of a list of index
  ///
  /// Return a tuple containg the elements of a tuple according to a
  /// list of indices
  template <int...Ints,
	    class...Tp>
  auto getIndexed(const IntSeq<Ints...>&,
		  const Tuple<Tp...>& tp)
  {
    return std::make_tuple(std::get<Ints>(tp)...);
  }
  
  /// Gets the head of a \c Tuple
  ///
  /// Return a tuple containg the first N elements of a tuple
  template <int N,
	    class...Tp>
  auto getHead(const Tuple<Tp...>& tp)     ///< Tuple from which to extract
  {
    return getIndexed(IntsUpTo<N>{},tp);
  }
  
  /// Gets the tail of a \c Tuple
  ///
  /// Return a \c Tuple containg the last N elements of a tuple
  template <int N,
	    class...Tp>
  auto getTail(const Tuple<Tp...>& tp)     ///< Tuple from which to extract
  {
    constexpr int tupleSize=sizeof...(Tp); /// Number of elements in the tuple
    constexpr int offset=tupleSize-N;      /// Beginning of returned part
    
    return getIndexed(RangeSeq<offset,1,tupleSize>{},tp);
  }
  
  /// Returns all elements of a \c Tuple but the N-th one
  ///
  /// Takes the list of component according to an index containing the
  /// first [0,N) and (N,Sizeof...(Tuple))
  ///
  /// Example:
  /// \code
  /// Tuple<int,double,char> e;
  /// auto GetAllBut<1>(e); // Tuple<int,char>
  /// \endcode
  template <int N,class...Tp>
  auto getAllBut(const Tuple<Tp...>& tp)
  {
    /// Number of elements in the tuple
    constexpr int tupleSize=sizeof...(Tp);
    
    return getIndexed(IntSeqCat<IntsUpTo<N>,RangeSeq<N+1,1,tupleSize>>{},tp);
  }
  
  /// Returns all elements of a \c Tuple but the type T
  ///
  /// First search the component, then call \c GetAllBut with the
  /// found position as a parameter
  ///
  /// Example:
  /// \code
  /// Tuple<int,double,char> e;
  /// auto GetAllBut<double>(e); // Tuple<int,char>
  /// \endcode
  template <class T,
	    class...Tp>
  auto getAllBut(const Tuple<Tp...>& tp)
  {
    return getAllBut<posOfType<T,Tuple<Tp...>>>(tp);
  };
}

#endif
