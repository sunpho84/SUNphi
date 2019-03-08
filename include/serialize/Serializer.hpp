#ifndef _SERIALIZE_SERIALIZER_HPP
#define _SERIALIZE_SERIALIZER_HPP

/// \file serialize/Serializer.hpp
///
/// \brief Used to serialize quantities
///
/// Incapsulates yaml access providing a stream operator

#include <yaml-cpp/yaml.h>

#include <metaprogramming/SFINAE.hpp>
#include <serialize/Scalar.hpp>

namespace YAML
{
  using namespace SUNphi;
  
  /// Serializable scalar conversion to YAML node
  template <typename T>
  struct convert<T,
		 EnableIfIsSerializableScalar<T>>
  {
    /// Encode a SerializableScalar
    static Node encode(const SerializableScalar<T>& rhs) ///< Input
    {
      /// Output node
      Node node;
      node<<~rhs;
      
      return
	node;
    }
    
    /// Decodes a SerializableScalar
    static bool decode(const Node& node,             ///< Input node
		       SerializableScalar<T>& rhs)   ///< Output
    {
      node>>rhs.a;
      
      return
	true;
    }
  };

  /// Serializable class conversion to YAML node
  template <typename T>
  struct convert<T,
		 EnableIfIsSerializableClass<T,BaseSerializableClass>>
  {
    /// Encode a SerializableClass
    static Node encode(const T& rhs) ///< Input
    {
      /// Output node
      Node node;
      node<<~rhs;
      
      return
	node;
    }
    
    /// Decodes a SerializableClass
    static bool decode(const Node& node,             ///< Input node
		       const T& rhs)                 ///< Output
    {
      node>>rhs.a;
      
      return
	true;
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
    
    /// Output a serializable object to a serializer
    template <typename T>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      if constexpr(isSerializableClass<T>)
	forEach(t.serializableMembers,
		[&ser](auto s)
		{
		  Serializer nested;
		  nested<<s();
		  
		  ser.node=
		    nested.node;
		});
      else
	if constexpr(isSerializableScalar<T>)
	  {
	    Serializer nested;
	    nested<<t();
	    
	    ser.node[t.name]=
	      nested.node;
	  }
	else
	  ser.node=t;
      
      return
	ser;
    }
    
    /// Returns the associated string
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
