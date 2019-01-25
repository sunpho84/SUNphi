#ifndef _STRING_HPP
#define _STRING_HPP

/// \file String.hpp
///
/// \brief Header file to support string operations

#include <string>

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
  
  /// Provides a static \c name() method to a class, returning STR
#define PROVIDE_NAME(STR)						\
  /*! Returns the name of the type */					\
  static constexpr const char* name()					\
  {									\
    return								\
      STR;								\
  }
  
  /// Returns the c-string of the passed quantity
  ///
  /// Case in which the input is already a c-string
  inline const char* cString(const char* in) ///< Input
  {
    return
      in;
  }
  
  /// Returns the c-string of the passed quantity
  ///
  /// Case in which the input is a c++ string
  inline const char* cString(const std::string& in) ///< Input
  {
    return
      in.c_str();
  }
  
}

#endif
