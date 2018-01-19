#ifndef _TUPLETYPECAT_HPP
#define _TUPLETYPECAT_HPP

/// \file TupleTypeCat.hpp
///
/// \brief Used to detect the type of catting a number of Tuple

#include <tuple/TupleWrap.hpp>

namespace SUNphi
{
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
  template <class...Tp>
  using TupleTypeCatT=decltype(tuple_cat(TupleWrap<Tp>{}...));
}

#endif
