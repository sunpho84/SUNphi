#ifndef _TUPLEWRAP_HPP
#define _TUPLEWRAP_HPP

/// \file TupleWrap.hpp
///
/// \brief Header file to treat homogeneously types and tuples

#include <tuple>

namespace SUNphi
{
  /// Wraps a simple type into a tuple containing the tuple
  ///
  /// For a generic type, put the type into a simple tuple, so that
  /// one can then treat the type homogeneously with other tuple with
  /// e.g. TupleTypeCatT
  ///
  /// Example:
  /// \code
  /// typename TupleWrapImplT<int>::type test; //std::tuple<int>
  /// \endcode
  ///
  template <class T>
  struct TupleWrapImplT
  {
    typedef std::tuple<T> type; ///< Internal mapped type
  };
  
  /// Remap a tuple type into itself
  ///
  /// For a generic type, put the type into a simple tuple, so that
  /// one can then treat the type homogeneously with other tuple with
  /// e.g. TupleTypeCatT
  ///
  /// Example:
  /// \code
  /// typename TupleWrapImplT<tuple<int>>::type test; //std::tuple<int>
  /// \endcode
  ///
  template <class ... T>
  struct TupleWrapImplT<std::tuple<T...>>
  {
    typedef std::tuple<T...> type; ///< Internal mapped type
  };
  
  /// Put the type into a tuple, if the type is not already a tuple.
  ///
  /// The following situations are possible:
  ///
  /// \- A non-tuple type is T put inside a tuple<T>
  ///
  /// \- A tuple<T> is mapped to itself
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
