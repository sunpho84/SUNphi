#ifndef _TUPLEWRAP_HPP
#define _TUPLEWRAP_HPP

/// \file TupleWrap.hpp
///
/// \brief Take a Tuple or a single type and returns a Tuple

#include <tuple/TupleClass.hpp>

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
  /// typename TupleWrap<int>::type test; //Tuple<int>
  /// \endcode
  template <class T>
  struct _TupleWrap
  {
    /// Internal mapped type
    typedef Tuple<T> type;
  };
  
  /// Remap a \c Tuple type into itself
  ///
  /// Defines a type equal to the Tuple passed as a parameter.
  ///
  /// Example:
  /// \code
  /// typename TupleWrap<Tuple<int>>::type test; //Tuple<int>
  /// \endcode
  template <class...T>
  struct _TupleWrap<Tuple<T...>>
  {
    /// Internal mapped type
    typedef Tuple<T...> type;
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
  /// TupleWrap<int> test1; //sTuple<int>
  /// TupleWrap<Tuple<int>> test2; //Tuple<int>
  /// \endcode
  template <class T>
  using TupleWrap=typename _TupleWrap<T>::type;
}

#endif
