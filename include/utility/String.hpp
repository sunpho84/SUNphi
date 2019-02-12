#ifndef _STRING_HPP
#define _STRING_HPP

/// \file String.hpp
///
/// \brief Header file to support string operations

#include <cstring>
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
  
  /// Returns a substring out of the passed chars, among position beg and end
  inline std::string substrBetweenPos(const std::string& str,    ///< Substring to split
				      const unsigned int& beg,   ///< Beginning position
				      const unsigned int& end)   ///< End position
  {
    /// Length to return
    const unsigned len=
      end-beg;
    
    return
      (end>beg)?
      str.substr(beg,len):
      "";
  }
  
  /// Length of a char
  constexpr unsigned int __strLength(char)
  {
    return
      1;
  }
  
  /// Length of a std::string
  inline unsigned int __strLength(const std::string& str)
  {
    return
      str.length();
  }
  
  /// Length of a c-string
  inline unsigned int __strLength(const char* str)
  {
    return
      strlen(str);
  }
  
  /// Returns a substring out of the passed delimiters
  template <typename T>
  inline std::string substrBetween(const std::string& str, ///< Substring to split
				   const T& i,             ///< Beginning token
				   const T& f)             ///< End token
  {
    /// Position of beginning token
    const unsigned int iPos=
      str.find(i);
    
    /// Beginning of the substring
    const unsigned int beg=
      (iPos==std::string::npos)?
      iPos+__strLength(i):
      iPos;
    
    /// End of the substring
    const unsigned int end=
      str.find(f);
    
    return
      substrBetweenPos(str,beg,end);
  }
}

#endif
