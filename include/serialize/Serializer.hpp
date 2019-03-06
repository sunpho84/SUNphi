#ifndef _SERIALIZE_SERIALIZER_HPP
#define _SERIALIZE_SERIALIZER_HPP

/// \file serialize/Serializer.hpp
///
/// \brief Used to serialize quantities
///
/// Incapsulates yaml access providing a stream operator

#include <yaml-cpp/yaml.h>

#include <serialize/Scalar.hpp>

namespace YAML
{
  using namespace SUNphi;
  
  template<typename T,
	   typename=EnableIf<isSerializableClass<T>>>
  using enable_if_serializableClass=
    T;
  
  //   template<template<typename...> class TT, typename... Ts>
  //   struct convert<enable_if_serializableScalar<TT<Ts...>>>
  // { 
  // };
  
  /// Serializable scalar conversion to YAML node
  template<template<typename...> class TT, typename... Ts>
  struct convert<enable_if_serializableClass<TT<Ts...>>>
  {
    using T=
      TT<Ts...>;
    
    static Node encode(const SerializableScalar<T>& rhs)
    {
      Node node;
      node<<~rhs;
      
      return node;
    }
    
    static bool decode(const Node& node,SerializableScalar<T>& rhs)
    {
      node>>rhs.a;
      
      return true;
    }
  };
}

namespace SUNphi
{
  /// Convert to string the passed objects
  class Serializer
  {
    /// Node used to store the serialized data
    YAML::Node node;
    
  public:
    
    template <typename T,
	      typename TDef>
    friend Serializer& operator<<(Serializer& ser,
				  const SerializableScalar<T,TDef>& t);
    
    template <typename T,
	      typename=EnableIf<not isSerializableScalar<T>>>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      if constexpr(isSerializableClass<T>)
		    forEach(t.serializableMembers,
			    [&ser](auto s)
			    {
			      ser.node<<s;
			    });
      else
	ser.node=t;
      
      return
	ser;
    }
    
    template <typename T,
	      typename TDef>
    friend Serializer& operator<<(Serializer& ser,
				  const SerializableScalar<T,TDef>& t)
    {
      ser.node[t.name]<<t();
      
      return
    ser;
    }
    
    std::string get()
      const
    {
      YAML::Emitter emitter;
      emitter<<node;
      
      return
	emitter.c_str();
    }
  };
}

#endif
