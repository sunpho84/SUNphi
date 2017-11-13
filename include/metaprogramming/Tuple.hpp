#ifndef _TUPLE_HPP
#define _TUPLE_HPP

/// \file Tuple.hpp
///
/// \brief Define Tuple

#include <tuple>

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
  
  /// Counts the same types
  ///
  /// Single pair of types case
  ///
  template<class T1,class T2>
  static constexpr int nOfType=IsSame<T1,T2>;
  
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
  
  /// Wraps a simple type into a \c Tuple containing the type
  ///
  /// For a generic type, put the type into a simple \c Tuple, so that
  /// it can be treated homogeneously with other Tuples in
  /// e.g. \c TupleTypeCatT
  ///
  /// Example:
  /// \code
  /// typename TupleWrapImplT<int>::type test; //Tuple<int>
  /// \endcode
  ///
  template <class T>
  struct TupleWrapImplT
  {
    typedef Tuple<T> type; ///< Internal mapped type
  };
  
  /// Remap a \c Tuple type into itself
  ///
  /// Defines a type equal to the Tuple passed as a parameter.
  ///
  /// Example:
  /// \code
  /// typename TupleWrapImplT<Tuple<int>>::type test; //Tuple<int>
  /// \endcode
  ///
  template <class...T>
  struct TupleWrapImplT<Tuple<T...>>
  {
    typedef Tuple<T...> type; ///< Internal mapped type
  };
  
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
  /// TupleWrapT<int> test1; //sTuple<int>
  /// TupleWrapT<Tuple<int>> test2; //Tuple<int>
  /// \endcode
  ///
  template <class T>
  using TupleWrapT=typename TupleWrapImplT<T>::type;
  
  /////////////////////////////////////////////////////////////////
  
  /// Gets element I from the tuple.
  ///
  /// Directly aliasing the std library. L-value case.
  /// \return A reference to the I tuple element.
  ///
  template<std::size_t I,class...Types>
  constexpr std::tuple_element_t<I,Tuple<Types...>>&
  Get(Tuple<Types...>& t) ///< Tuple from which extract
  {return std::get<I>(t);}
  
  /// Gets an element from the tuple.
  ///
  /// Directly aliasing the std library. R-value case
  /// \return A move-reference to the I tuple element.
  ///
  template<std::size_t I,class...Types>
  constexpr std::tuple_element_t<I,Tuple<Types...>>&&
  Get(Tuple<Types...>&& t) ///< Tuple from which extract
  {return std::get<I>(t);}
  
  /// Gets the element of type T from the tuple.
  ///
  /// Directly aliasing the std library. L-value case.
  /// \return A reference to the T tuple type (if present).
  ///
  template<class T,class...Types>
  constexpr T& Get(Tuple<Types...>& t) ///< Tuple from which extract
  {return std::get<T>(t);}
  
  /// Gets the element of type T from the tuple.
  ///
  /// Directly aliasing the std library. R-value case.
  /// \return A move-reference to the T tuple type (if present).
  ///
  template<class T,class...Types>
  constexpr T&& Get(Tuple<Types...>&& t) ///< Tuple from which extract
  {return std::get<T>(t);}
  
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
  using TupleTypeCatT=decltype(tuple_cat(TupleWrapT<Tp>{}...));
  
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
  ForEach(const Tuple<Tp...>& t ///< \c Tuple to act upon
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
  VoidIf<(I<sizeof...(Tp))>
  ForEach(Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)        ///< \c Function iterating on the \c Tuple
  {
    f(Get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
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
  VoidIf<(I<sizeof...(Tp))>
  ForEach(const Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)              ///< \c Function iterating on the \c Tuple
  {
    f(Get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
}

#endif
