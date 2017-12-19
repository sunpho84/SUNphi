#ifndef _STRING_HPP
#define _STRING_HPP

#include <metaprogramming/IntSeq.hpp>

/// \file String.hpp
///
/// \brief Header file to support string operations

namespace SUNphi
{
  /// Provides a string as member of a struct
  ///
  /// The list of chars is provided as template argument
  template <char...Chars>
  struct IncapsulatedString
  {
    /// List of chars passed as a template in a char array
    const char str[sizeof...(Chars)]{Chars...};
  };

  template <int...Ids>
  decltype(auto) incapsulateString(const char *str,IntSeq<Ids...>)
  {
    return IncapsulatedString<str[Ids]...>();
  }
  
  
}

#endif
