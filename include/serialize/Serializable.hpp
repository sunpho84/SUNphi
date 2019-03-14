#ifndef _SERIALIZABLE_HPP
#define _SERIALIZABLE_HPP

/// \file Serializable.hpp
///
/// \brief Provides a class with serializability features via CRTP

#include <string>

#include <metaprogramming/CRTP.hpp>

namespace SUNphi
{
  /// Provides a class with serializability features via CRTP
  template <typename T>
  class Serializable
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
  public:
    
    /// Creates the string
    std::string serialize()
      const;
  };
}

#endif
