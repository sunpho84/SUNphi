#ifndef _TUPLEWRAP_HPP
#define _TUPLEWRAP_HPP

/// \file TupleWrap.hpp
///
/// \brief Header file to treat homogeneously types and tuples

#include <metaprogramming/Tuple.hpp>

namespace SUNphi
{
  /// Wraps a simple type into a \c tuple containing the type
  ///
  /// For a generic type, put the type into a simple \c tuple, so that
  /// it can be treated homogeneously with other tuples in
  /// e.g. \c TupleTypeCatT
  ///
  /// Example:
  /// \code
  /// typename TupleWrapImplT<int>::type test; //std::tuple<int>
  /// \endcode
  ///
  template <class T>
  struct TupleWrapImplT
  {
    typedef Tuple<T> type; ///< Internal mapped type
  };
  
  /// Remap a \c tuple type into itself
  ///
  /// For a generic type, put the type into a simple \c tuple, so that
  /// it can be treated homogeneously with other tuples in
  /// e.g. \c TupleTypeCatT
  ///
  /// Example:
  /// \code
  /// typename TupleWrapImplT<tuple<int>>::type test; //std::tuple<int>
  /// \endcode
  ///
  template <class...T>
  struct TupleWrapImplT<Tuple<T...>>
  {
    typedef Tuple<T...> type; ///< Internal mapped type
  };
  
  /// Put the type into a \c tuple, if the type is not already a \c tuple.
  ///
  /// The following situations are possible:
  ///
  /// \li A non-tuple type \c T is put inside a \c tuple<T>
  ///
  /// \li A \c tuple<T> is mapped to itself
  ///
  /// Example:
  /// \code
  /// TupleWrapT<int> test1; //std::tuple<int>
  /// TupleWrapT<tuple<int>> test2; //std::tuple<int>
  /// \endcode
  ///
  template <class T>
  using TupleWrapT=typename TupleWrapImplT<T>::type;
}

#endif
