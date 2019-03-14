#ifndef _SERIALIZE_SERIALIZER_HPP
#define _SERIALIZE_SERIALIZER_HPP

/// \file serialize/Serializer.hpp
///
/// \brief Used to serialize quantities
///
/// Incapsulates yaml access providing an output stream operator

#include <yaml-cpp/yaml.h>

#include <metaprogramming/SFINAE.hpp>
#include <serialize/Base.hpp>
#include <serialize/Scalar.hpp>
#include <serialize/Serializable.hpp>

namespace SUNphi
{
  /// Convert to string the passed objects
  template <bool Ref=false>
  class Serializer
  {
    /// Node used to store the serialized data
    RefIf<Ref,YAML::Node> node;
    
    /// Writes everything or the default only
    const bool onlyNonDef{false};
    
  public:
    
    /// Output a non-serializable object
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(not (isSerializableClass<T> or isSerializableScalar<T>))>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      ser.node=
	t;
      
      return
	ser;
    }
    
    /// Output a serializable scalar
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableScalar<T>)>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      if(not (ser.onlyNonDef and t.isDefault()))
	{
	  /// Creates the node
	  auto rc=
	    ser.node[t.name];
	  
	  /// Creates the nested serializer, and fills it
	  Serializer<true> nested(rc,ser.onlyNonDef);
	  nested<<
	    t();
	}
      
      return
	ser;
    }
    
    /// Output a serializable class
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableClass<T>)>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      forEach(t.serializableMembers(),
	      [&ser](auto s)
	      {
		ser<<
		  s;
	      });
      
      return
	ser;
    }
    
    /// Returns the associated string
    std::string get()
      const
    {
      /// Emits the string
      YAML::Emitter emitter;
      emitter<<
	node;
      
      return
	emitter.c_str();
    }
    
    /// Creates a serializer
    Serializer(const bool& onlyNonDef=false) :
      onlyNonDef(onlyNonDef)
    {
    }
    
    /// Creates a serializer
    Serializer(YAML::Node& node,
	       const bool& onlyNonDef=false) :
      node(node),
      onlyNonDef(onlyNonDef)
    {
    }
  };
  
  /// Creates a string out of a serializable
  template <typename T>
  std::string Serializable<T>::serialize()
      const
  {
    /// Serializer used to create the string
    Serializer ser;
    
    ser<<
      (~(*this));
    
    return
      ser.get();
  }
  
  /// Stream operator using serialize method
  template <typename S,
	    typename T>
  S& operator<<(S&& stream,
		const Serializable<T>& ser)
    {
      return
	stream<<
	  ser.serialize();
    }
  
}

#endif
