#ifndef _TUPLE_HPP
#define _TUPLE_HPP

/// \file Tuple.hpp
///
/// \brief Define Tuple

#include <tuple>

#include <metaprogramming/IntSeq.hpp>
#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Define Tuple, a list of arbitrary types.
  ///
  /// Directly aliasing the std library.
  ///
  template<class...Tp>
  using Tuple=std::tuple<Tp...>;
  
  /////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Check that a type is a tuple
    ///
    /// Generic false case
    ///
    template <class T>
    struct IsTuple : public FalseType
    {
    };
    
    /// Check that a type is a tuple
    ///
    /// Specialization for a true tuple
    ///
    template <class...Tp>
    struct IsTuple<Tuple<Tp...>> : public TrueType
    {
    };
  }
  
  /// Check that a type is a tuple
  ///
  /// Gives visibility to the internal implementation
  ///
  template <class T>
  constexpr int IsTuple=Impl::IsTuple<T>::value;
  
  /// Counts the same type
  ///
  /// Counts the occurrency of type T inside a tuple
  ///
  template<class T,class...Tp>
  static constexpr int nOfType<T,Tuple<Tp...>> =hSum<IsSame<T,Tp>...>;
  
  /// Counts the same type
  ///
  /// Wrapper to switch tuple and searched type
  ///
  template<class T,class...Tp>
  static constexpr int nOfType<Tuple<Tp...>,T> =nOfType<T,Tp...>;
  
  /////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Wraps a simple type into a \c Tuple containing the type
    ///
    /// For a generic type, put the type into a simple \c Tuple, so that
    /// it can be treated homogeneously with other Tuples in
    /// e.g. \c TupleTypeCatT
    ///
    /// Example:
    /// \code
    /// typename TupleWrap<int>::type test; //Tuple<int>
    /// \endcode
    ///
    template <class T>
    struct TupleWrap
    {
      typedef Tuple<T> type; ///< Internal mapped type
    };
    
    /// Remap a \c Tuple type into itself
    ///
    /// Defines a type equal to the Tuple passed as a parameter.
    ///
    /// Example:
    /// \code
    /// typename TupleWrap<Tuple<int>>::type test; //Tuple<int>
    /// \endcode
    ///
    template <class...T>
    struct TupleWrap<Tuple<T...>>
    {
      typedef Tuple<T...> type; ///< Internal mapped type
    };
  }
  
  /// Put the type into a \c Tuple, if the type is not already a \c Tuple.
  ///
  /// The following situations are possible:
  ///
  /// \li A non-Tuple type \c T is put inside a \c Tuple<T>
  ///
  /// \li A \c Tuple<T> is mapped to itself
  ///
  /// Example:
  /// \code
  /// TupleWrap<int> test1; //sTuple<int>
  /// TupleWrap<Tuple<int>> test2; //Tuple<int>
  /// \endcode
  ///
  template <class T>
  using TupleWrap=typename Impl::TupleWrap<T>::type;
  
  /////////////////////////////////////////////////////////////////
  
  /// Gets element I from the tuple.
  ///
  /// Directly aliasing the std library. L-value case.
  /// \return A reference to the I tuple element.
  ///
  template<std::size_t I,class...Types>
  constexpr std::tuple_element_t<I,Tuple<Types...>>&
  get(Tuple<Types...>& t) ///< Tuple from which extract
  {
    return std::get<I>(t);
  }
  
  /// Gets an element from the tuple.
  ///
  /// Directly aliasing the std library. R-value case
  /// \return A move-reference to the I tuple element.
  ///
  template<std::size_t I,class...Types>
  constexpr std::tuple_element_t<I,Tuple<Types...>>&&
  get(Tuple<Types...>&& t) ///< Tuple from which extract
  {
    return std::get<I>(t);
  }
  
  /// Gets the element of type T from the tuple.
  ///
  /// Directly aliasing the std library. L-value case.
  /// \return A reference to the T tuple type (if present).
  ///
  template<class T,class...Types>
  constexpr T& get(Tuple<Types...>& t) ///< Tuple from which extract
  {
    return std::get<T>(t);
  }
  
  /// Gets the element of type T from the tuple.
  ///
  /// Directly aliasing the std library. R-value case.
  /// \return A move-reference to the T tuple type (if present).
  ///
  template<class T,class...Types>
  constexpr T&& get(Tuple<Types...>&& t) ///< Tuple from which extract
  {
    return std::get<T>(t);
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Define a \c tuple catting all tuple parameter types, and plain
  /// types of all parameters
  ///
  /// Creates a \c tuple from all passed parameters. If an argument is
  /// a tuple, consider its parameter types. Otherwise consider the
  /// type itself.
  ///
  /// Example:
  /// \code
  /// typedef Tuple<int,double> T;
  /// TupleTypeCatT<T,T> test1;    //Tuple<int,double,int,double>
  /// TupleTypeCatT<T,char> test2; //Tuple<int,double,char>
  /// \endcode
  ///
  template <class...Tp>
  using TupleTypeCatT=decltype(tuple_cat(TupleWrap<Tp>{}...));
  
  /////////////////////////////////////////////////////////////////
  
  /// Loop over all \c Tuple elements
  ///
  /// Terminator of recursive call, called automaticaly also when the
  /// Tuple is empty.
  ///
  /// \tparam I Index of the \c Tuple element to extract
  /// \tparam Func \c Function type
  /// \tparam Tp... The Tuple parameters
  /// \return \c void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<I==sizeof...(Tp)>
  forEach(const Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)              ///< \c Function iterating on the Tuple
  {
  }
  
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
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<(sizeof...(Tp)>I)>
  forEach(Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)        ///< \c Function iterating on the \c Tuple
  {
    f(get<I>(t));
    forEach<I+1,Func,Tp...>(t,f);
  }
  
  /// Loop over all \c tuple elements
  ///
  /// Const access to all elements of the \c Tuple, called recursively
  /// until I==sizeof...(Tp), incrementing the parameter I.
  ///
  /// \tparam I Index of the \c Tuple element to extract
  /// \tparam Func \c Function type
  /// \tparam Tp... The \c Tuple parameters
  /// \return \c void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<(sizeof...(Tp)>I)>
  forEach(const Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)              ///< \c Function iterating on the \c Tuple
  {
    f(get<I>(t));
    forEach<I+1,Func,Tp...>(t,f);
  }
  
  /////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Gets the position of a type in a list
    ///
    /// Forward definition
    ///
    template <class T,class...Tp>
    struct PosOfType;
    
    /// Gets the position of a type in a list
    ///
    /// Case of matching type
    ///
    template <class T,class...Tp>
    struct PosOfType<T,T,Tp...>
    {
      static_assert(hSum<IsSame<T,Tp>...> ==0,"Multiple occurrency of the searched type");
      static constexpr int value=0; ///< Set the position to 0, the first of the list
    };
    
    /// Gets the position of a type in a list
    ///
    /// Case of non-matching type, instantiate iterativerly the searcher
    ///
    template <class T,class U,class...Tp>
    struct PosOfType<T,U,Tp...>
    {
      static_assert(sizeof...(Tp),"Type not found in the list");
      static constexpr int value=1+PosOfType<T,Tp...>::value; ///< Set the position to one more than the nested value
    };
    
    /// Gets the position of a type in a tuple
    ///
    /// Wraps the ordinary list searcher
    ///
    template <class T,class...Tp>
    struct PosOfType<T,Tuple<Tp...>>
    {
      static constexpr int value=PosOfType<T,Tp...>::value; ///< Position inside the list
    };
  }
  
  /// Gets the position of a type in a tuple
  ///
  /// Wraps the actual implementation
  ///
  /// \code
  /// int a=PosOfType<int, int,double,char>;        //0
  /// int b=PosOfType<int, Tuple<int,double,char>>; //0
  /// int c=PosOfType<int, char,double,char>;       //static_assert (not found)
  /// int d=PosOfType<int, int,int,char>;           //static_assert (multiple occurency)
  /// \endcode
  ///
  template <class...Tp>
  constexpr int posOfType=Impl::PosOfType<Tp...>::value;
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Filter a \c Tuple on the basis of a list of index
  ///
  /// Return a tuple containg the elements of a tuple according to a
  /// list of indices
  ///
  template <int...Ints,class...Tp>
  auto getIndexed(const IntSeq<Ints...>&,const Tuple<Tp...> &tp)
  {
    return std::make_tuple(std::get<Ints>(tp)...);
  }
  
  /// Gets the head of a \c Tuple
  ///
  /// Return a tuple containg the first N elements of a tuple
  ///
  template <int N,class...Tp>
  auto getHead(const Tuple<Tp...> &tp)     ///< Tuple from which to extract
  {
    return getIndexed(IntsUpTo<N>{},tp);
  }
  
  /// Gets the tail of a \c Tuple
  ///
  /// Return a \c Tuple containg the last N elements of a tuple
  ///
  template <int N,class...Tp>
  auto getTail(const Tuple<Tp...> &tp)     ///< Tuple from which to extract
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
  ///
  template <int N,class...Tp>
  auto getAllBut(const Tuple<Tp...> &tp)
  {
    constexpr int tupleSize=sizeof...(Tp); ///< Number of elements in the tuple
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
  ///
  template <class T,class...Tp>
  auto getAllBut(const Tuple<Tp...> &tp)
  {
    return getAllBut<posOfType<T,Tuple<Tp...>>>(tp);
  };
}

#endif
