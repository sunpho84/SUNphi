#ifndef _TUPLETYPECAT_HPP
#define _TUPLETYPECAT_HPP

#include <metaprogramming/TupleWrap.hpp>

/// \file TupleTypeCat.hpp
///
/// \brief Header file to cat tuple types

namespace SUNphi
{
  /// Define a tuple catting all parameter types
  ///
  /// Creates a tuple from all passed parameters. If an argument is a
  /// tuple, consider its parameter types. Otherwise consider the type itself.
  ///
  /// Example:
  /// \code
  /// typedef tuple<int,double> T;
  /// TupleTypeCatT<T,T> test1;    //std::tuple<int,double,int,double>
  /// TupleTypeCatT<T,char> test2; //std::tuple<int,double,char>
  /// \endcode
  ///
  template <class ... Tp>
  using TupleTypeCatT=decltype(tuple_cat(TupleWrapT<Tp>{}...));
}

#endif
