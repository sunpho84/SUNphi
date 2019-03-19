#ifndef _DEFAULT_HPP
#define _DEFAULT_HPP

/// \file serialize/Default.hpp
///
/// \brief Provides default for a serializer

#include <metaprogramming/UniversalReferences.hpp>
#include <serialize/Base.hpp>

namespace SUNphi
{
  /// Used to mark that no default value is provided
  [[ maybe_unused ]]
  static constexpr bool NO_DEFAULT=
    false;
  
  /// Used to mark the mandatoriety of the variable
  [[ maybe_unused ]]
  constexpr bool MANDATORY=
    false;
  
  template <typename T,
	    bool>
  class SerializableDefaultValue;
  
  /// Provide a default value for a serializable reference
  template <typename T>
  class SerializableDefaultValue<T,true>
  {
  public:
    
    /// Default value stored
    const T def;
    
    /// Store the default value
    template <typename...TDef>
    SerializableDefaultValue(TDef&&...def)
      : def(forw<TDef>(def)...)
    {
      static_assert(not isSerializableClass<T>,"A serializable class has already its own defaults");
    }
  };
  
  /// Provide no default value for a serializable reference
  template <typename T>
  class SerializableDefaultValue<T,false>
  {
  public:
    
    /// Construct doing nothing
    SerializableDefaultValue(const T&)
    {
    }
  };
}

#endif
