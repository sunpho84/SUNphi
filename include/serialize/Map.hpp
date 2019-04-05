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
#include <serialize/Serializable.hpp>
#include <tuple/TupleClass.hpp>

#include <ios/Logger.hpp>

namespace SUNphi
{
  /// Helper function to create the serializable members
  template <typename...Ts>
  auto _serializableMembers(Ts&&...ts)
  {
    /// Check that all arguments are SerializableScalar
    static_assert((isSerializable<Ts> && ...),"All arguments need to be Serializable");
    
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
    
    /// Check if the class is default
    bool isDefault()
      const
    {
      /// Default value
      bool def=
	true;
      
      forEach(CRTP_THIS.serializableMembers(),
	      [&def](auto& s)
	      {
		def&=
		  s.isDefault();
	      });
      
      return
	def;
    }
    
    /// Convert to YAML node
    YAML::Node serialize(YAML::Node& node,                 ///< Resulting node
			 const bool& onlyNonDefault=false) ///< Prints only non-default
      const
    {
      forEach(CRTP_THIS.serializableMembers(),
	      [&node,&onlyNonDefault](auto& s)
	      {
		s.serialize(node,onlyNonDefault);
	      });
      
      return
	node;
    }
    
    /// Convert to YAML node
    YAML::Node serialize(const bool& onlyNonDefault=false) ///< Prints only non-default
      const
    {
      /// Resulting node
      YAML::Node node;
      
      return
	serialize(node,onlyNonDefault);
    }
    
    /// Put the class to the default value
    void putToDefault()
    {
      forEach(CRTP_THIS.serializableMembers(),
	      [](auto& s)
	      {
		s.putToDefault();
	      });
    }
    
    /// Check that no spurious subnodes are contained
    void checkNoSpuriousSubNodes(const YAML::Node& node) ///< Node to be checked
    {
      /// Spurious list
      std::string spurious;
      
      for(auto& subNode : node)
	{
	  /// Mark found or not
	  bool found=
	    false;
	  
	  /// Expected name
	  const std::string name=
	    subNode.first.as<std::string>();
	  
	  forEach(CRTP_THIS.serializableMembers(),
		  [name,&found](auto& s)
		  {
		    found|=
		      s.name==name;
		  });
	  
	  if(not found)
	    {
	      if(spurious.size())
		spurious+=
		  ", ";
	      
	      spurious+=
		name;
	    }
	}
      
      if(spurious.size())
	CRASH<<"Spurious tags: "<<spurious;
    }
    
    /// Convert from a YAML node
    bool deSerialize(const YAML::Node& node) ///< Node from which to convert
    {
      if(not node.IsMap())
	CRASH<<"Needs to be a map, is: "<<(node.IsNull()?"Null":(node.IsScalar()?"Scalar":(node.IsSequence()?"Sequence":"Unknown")));
      
      forEach(CRTP_THIS.serializableMembers(),
	      [&node](auto& s)
	      {
		s.deSerialize(node);
	      });
      
      checkNoSpuriousSubNodes(node);
      
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
#define DEFINE_SERIALIZABLE_CLASS(T)		\
  class T :					\
    public SerializableClass<T>
  
  /// Defines a serializable class
#define SERIALIZABLE_CLASS(TYPE,NAME)		\
  Serializable<TYPE> NAME{#NAME}
  
  /// Defines a list of serializable members
#define LIST_SERIALIZABLE_MEMBERS(...)					\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(     , __VA_ARGS__);		\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(const, __VA_ARGS__);
}

#endif
