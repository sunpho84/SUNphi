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

#include <iostream>
namespace SUNphi
{
  /// Convert to string the passed objects
  class Serializer
  {
    /// Node used to store the serialized data
    YAML::Node node;
    
    /// Writes everything or the default only
    const bool onlyNonDef{false};
    
  public:
    
    /// Output a serializable object to a serializer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(not (isSerializableClass<T> or isSerializableScalar<T>))>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
            std::cout<<"nothing"<<std::endl;

      ser.node=
	t;
      
      return
	ser;
    }
    
    /// Output a serializable object to a serializer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableClass<T>)>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      forEach(t.serializableMembers,
	      [&ser](auto s)
	      {
		ser<<
		  s;
	      });
      
      return
	ser;
    }
    
    /// Output a serializable object to a serializer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableScalar<T>)>
    friend Serializer& operator<<(Serializer& ser,
				  const T& t)
    {
      if(not (ser.onlyNonDef and t.isDefault()))
	{
	  std::cout<<"scalar "<<t.name<<" "<<ser.onlyNonDef<<" "<<t.isDefault()<<std::endl;
      
	  /// Creates the nested serializer
	  Serializer nested(ser.onlyNonDef);
	  nested<<
	    t();
	  
	  ser.node[t.name]=
	    nested.node;
	}
      
      return
	ser;
    }
    
    /// Returns the associated string
    std::string get()
      const
    {
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
  };
}

#endif
