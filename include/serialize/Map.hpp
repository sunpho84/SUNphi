#ifndef _SERIALIZE_MAP_HPP
#define _SERIALIZE_MAP_HPP

/// \file serialize/Map.hpp
///
/// \brief Creates a list of a set of serializable scalars
///
/// The list is a tuple, which can be added as a member to a class, to
/// make the class itself serializable

#include <yaml-cpp/yaml.h>

#include <metaprogramming/CRTP.hpp>
#include <serialize/Base.hpp>
#include <serialize/Scalar.hpp>
#include <tuple/TupleClass.hpp>

#include <ios/Logger.hpp>

namespace SUNphi
{
  /// Helper function to create the serializable members
  template <typename...Ts>
  auto _serializableMembers(Ts&&...ts)
  {
    /// Check that all arguments are SerializableScalar
    static_assert((isSerializableScalar<Ts> && ...),"All arguments need to be SerializableScalar");
    
    return
      std::forward_as_tuple(ts...);
  }
  
  /// Provides the class embedding with a serializableMemebers method
#define CONST_OR_NOT_SERIALIZABLE_MEMBERS(CONST,			\
					  ...)				\
  /*! Gets the reference to the serializable members */			\
  auto serializableMembers()						\
    CONST								\
  {									\
    return								\
      std::forward_as_tuple(__VA_ARGS__);				\
  } 									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Provides serialization features to a class
  template <typename T>
  class SerializableClass
  {
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Convert to YAML node
    YAML::Node serialize()
      const
    {
      /// Resulting node
      YAML::Node node;
      
      forEach(CRTP_THIS.serializableMembers(),
	      [&node](auto& s)
	      {
		node[s.name]=
		  s();
	      });
      
      return
	node;
    }
    
    /// Convert from a YAML node
    bool deSerialize(const YAML::Node& node) ///< Node from which to convert
    {
      if(not node.IsMap())
	CRASH<<"Needs to be a map, is: "<<(node.IsNull()?"Null":(node.IsScalar()?"Scalar":(node.IsSequence()?"Sequence":"Unknown")));
      
      forEach(CRTP_THIS.serializableMembers(),
	      [&node](auto& s)
	      {
		/// Resulting type
		using R=
		  RemRef<decltype(s())>;
		
		runLog()<<"Searching for "<<s.name<<" in node of tag "<<node.Tag();
		
		if(not node[s.name])
		  {
		    YAML::Emitter e;
		    e<<node;
		    CRASH<<"Unable to find "<<s.name<<", was: \n"<<e.c_str();
		  }
		else
		  runLog()<<"Found "<<s.name;
		s()=
		  node[s.name].template as<R>();
	      });
      
      return
	true;
    }
    
    /// Convert to binary
    template <typename B=Binarizer>
    Binarizer& binarize(B&& out={})             ///< Output
      const
    {
      forEach(CRTP_THIS.serializableMembers(),
	      [&out](auto& s)
	      {
		out.binarize(s);
	      });
      
      return
	out;
    }
    
    /// Convert from binary
    template <typename B=Binarizer>
    Binarizer& deBinarize(B&& in)              ///< Input
    {
      forEach(CRTP_THIS.serializableMembers(),
	      [&in](auto& s)
	      {
		in.deBinarize(s);
	      });
      
      return
	in;
    }
  };
  
  /// Shortcut to define a serializable class
#define SERIALIZABLE_CLASS(T)			\
  class T :					\
    public SerializableClass<T>
  
  /// Defines a list of serializable members
#define LIST_SERIALIZABLE_MEMBERS(...)					\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(     , __VA_ARGS__);		\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(const, __VA_ARGS__);
}

#endif
