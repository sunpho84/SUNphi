#ifndef _TUPLEWRAP_HPP
#define _TUPLEWRAP_HPP

/// \file TupleWrap.hpp
///
/// \brief Treat homogeneously types and Tuples

#include <metaprogramming/Tuple.hpp>

namespace SUNphi
{
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
}

#endif
