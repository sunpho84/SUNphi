#ifndef _INTSEQ_HPP
#define _INTSEQ_HPP

/// \file IntSeq.hpp
///
/// \brief Implements a struct holding a sequence of integers.

namespace SUNphi
{
  /// Sum of all integers
  ///
  template <int Head,int...Tail>
  constexpr int Sum=Head+Sum<Tail...>;

  /// Sum of all integers, unary case
  ///
  template <int Head>
  constexpr int Sum<Head> =Head;
  
  /// A struct holding a sequence of integer (similar to stdlib).
  ///
  /// The \c integers are held as parameters of the class. To
  /// intercept them, use the class in a context in which the integer
  /// list are deduced as template parameter, as in this example:
  ///
  /// \code
  /// template <class T>
  /// class PutIntoArrays
  /// {
  /// };
  ///
  /// template <int...Ints>
  /// class PutIntoArrays<IntSeq<Ints...>>
  /// {
  ///    std::array<int,IntSeq<Ints...>::Size> arr{{Ints...}};
  /// };
  /// \endcode
  ///
  /// \tparam Ints the list of integer
  ///
  template <int...Ints>
  struct IntSeq
  {
    static constexpr int size=sizeof...(Ints); ///< Length of the sequence of integer
    static constexpr int sum=Sum<Ints...>; ///< Sum of all elements
  };
}

#endif
