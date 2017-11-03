#ifndef _TUPLE_HPP
#define _TUPLE_HPP

/// \file Tuple.hpp
///
/// \brief Define Tuple

#include <tuple>

/// Define Tuple, a list of arbitrary types.
///
/// Directly aliasing the std library.
///
template<class...Tp>
using Tuple=std::tuple<Tp...>;

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

#endif
