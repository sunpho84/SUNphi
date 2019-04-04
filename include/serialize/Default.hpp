#ifndef _SERIALIZE_DEFAULT_HPP
#define _SERIALIZE_DEFAULT_HPP

/// \file serialize/Default.hpp
///
/// \brief Provides default for a serializer

#include <metaprogramming/UniversalReferences.hpp>
#include <serialize/Base.hpp>

namespace SUNphi
{
  DEFINE_HAS_MEMBER(isDefault);
  
  /// Used to mark that no default value is provided
  [[ maybe_unused ]]
  static constexpr bool NO_DEFAULT=
    false;
  
  /// Used to mark the mandatoriety of the variable
  [[ maybe_unused ]]
  constexpr bool MANDATORY=
    false;
  
  /// Mark to print only non-default
  [[ maybe_unused ]]
  constexpr bool ONLY_NON_DEFAULT=
    true;
  
  /// Provide a default value for a serializable reference
  ///
  /// Forward declaration
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
    }
    
    /// Put to default
    void putToDefault()
    {
      (*static_cast<Serializable<T>*>(this))()=
	def;
    }
  };
  
  /// Provide no default value for a serializable reference
  template <typename T>
  class SerializableDefaultValue<T,false>
  {
  public:
    
    /// Construct doing nothing
    SerializableDefaultValue(const T={})
    {
    }
    
    /// Put to default
    void putToDefault()
    {
      (*static_cast<Serializable<T>*>(this))()=
	T{};
    }
  };
}

#endif
