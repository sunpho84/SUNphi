#ifndef _DEFAULT_HPP
#define _DEFAULT_HPP

/// \file serialize/Default.hpp
///
/// \brief Provides default for a serializer

#include <metaprogramming/UniversalReferences.hpp>
#include <serialize/Base.hpp>

namespace SUNphi
{
  /// Marks that the value has no default value
  class NoDefault
  {
  };
  
  /// Used to mark that no default value is provided
  [[ maybe_unused ]]
  static constexpr NoDefault NO_DEFAULT;
  
  /// Used to mark the mandatoriety of the variable
  [[ maybe_unused ]]
  static constexpr NoDefault MANDATORY;
  
  /// Provide a default value for a serializable reference
  template <typename T>
  class SerializableDefaultValue
  {
  public:
    
    /// Default value stored
    const T def;
    
    /// Store the default value
    SerializableDefaultValue<T>(const T& def)
      : def(def)
    {
      static_assert(not isSerializableClass<T>,"A serializable class has already its own defaults");
    }
  };
  
  /// Provide no default value for a serializable reference
  template <>
  class SerializableDefaultValue<NoDefault>
  {
  public:
    
    /// Construct doing nothing
    SerializableDefaultValue<NoDefault>(const NoDefault&)
    {
    }
  };
}

#endif
