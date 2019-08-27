#ifndef _SERIALIZE_BASE_HPP
#define _SERIALIZE_BASE_HPP

/// \file serialize/Base.hpp
///
/// \brief Defines the basic serializable feature
///
/// The value mapped can be of three types:
///
/// - a scalar, of a trivially copyable kind
/// - a map, namely a class with a "serializableMembers" member
/// - a sequence, namely a tuple-like or vector-like container


#include <yaml-cpp/yaml.h>

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Forward definition of serializable
  template <typename T>
  class Serializable;
  
  DEFINE_HAS_MEMBER(serializableMembers);
  
  /// Check if the class is a serializable map
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isSerializableMap=
    hasMember_serializableMembers<T>;
  
  DEFINE_HAS_MEMBER(serialize);
  
  /// Stream to an output a YAML node
  template <typename S,
	    SFINAE_ON_TEMPLATE_ARG(not canPrint<S,
				   const YAML::Node&>)>
  S& operator<<(S&& stream,
		const YAML::Node& node)
  {
    /// Converter to string
    YAML::Emitter emitter;
    emitter<<node;
    
    stream<<
      emitter.c_str();
    
    return
      stream;
  }
  /// Stream to an output anything with a \c serialize member
  template <typename S,
	    typename T,
	    SFINAE_ON_TEMPLATE_ARG(not canPrint<S,const T&>),
	    SFINAE_ON_TEMPLATE_ARG(hasMember_serialize<T>)>
  S& operator<<(S&& stream,
		const T& ser)
  {
    stream<<
      ser.serialize();
    
    return
      stream;
  }
}

namespace YAML
{
  using namespace SUNphi;
  
  /// Converter for any type providing member serialize
  template<typename T>
  struct convert<T,EnableIf<hasMember_serialize<T>>>
  {
    /// Encode into a node
    static Node encode(const T& rhs)
    {
      return
	rhs.serialize();
    }
    
    /// Decode from a node
    static bool decode(const Node& node,
		       T& rhs)
    {
      rhs.deSerialize(node);
      
      return
	true;
    }
  };
}

#endif
