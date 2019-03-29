#ifndef _SERIALIZABLE_HPP
#define _SERIALIZABLE_HPP

/// \file Serializable.hpp
///
/// \brief Provides a class with serializability features
///
/// The value mapped can be of three types:
///
/// - a scalar, of a trivially copyable kind
/// - a map, namely a class with a "serializableMembers" member
/// - a sequence, namely a tuple-like or vector-like container

#include <string>

#include <yaml-cpp/yaml.h>

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

#include <ios/Logger.hpp>

namespace SUNphi
{
  /// Check if the type is a serializable scalar
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isSerializableScalar=
    std::is_trivially_copyable_v<T>;
}

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
  };
  
  /// Shortcut to define a serializable class
#define SERIALIZABLE_CLASS(T)			\
  class T :					\
    public SerializableClass<T>
  
  /// Defines a list of serializable members
#define SERIALIZABLE_MEMBERS(...)					\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(     , __VA_ARGS__);		\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(const, __VA_ARGS__);
  DEFINE_HAS_MEMBER(serializableMembers);
  
  /// Check if the class is a serializable map
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isSerializableMap=
    hasMember_serializableMembers<T>;
  
  /// Used to mark that no default value is provided
  [[ maybe_unused ]]
  static constexpr bool NO_DEFAULT=
    false;
  
  /// Used to mark the mandatoriety of the variable
  [[ maybe_unused ]]
  constexpr bool MANDATORY=
    false;
  
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
      static_assert(not isSerializableMap<T>,"A serializable map has already its own defaults");
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
  
  /// Provides a class with serializability features via CRTP
  template <typename T,
	    bool HasDefault=true,
	    bool isScalar=SUNphi::isSerializableScalar<T>,
	    bool isMap=isSerializableMap<T>,
	    bool isTupleLike=SUNphi::isTupleLike<T>,
	    bool isVectorlike=SUNphi::isVectorLike<T>>
  class Serializable
    : public SerializableDefaultValue<T,HasDefault>
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Stored variable
    T value;
    
  public:
    
    /// Mapped type
    using Type=
      T;
    
    /// Access the value
    const T& operator()()
      const
    {
      return
	value;
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(operator());
    
    /// Name used to refer
    const char* name;
    
    /// Creates a serializable with default value
    Serializable(const char* name, ///< Name to be used
		 const T& def={})  ///< Default, if passed
      :
      SerializableDefaultValue<T,HasDefault>(def),
      name(name)
    {
      static_assert((not isMap) or (not HasDefault),"A serializable class has his own default members");
      
      value=
	def;
    }
    
    /// Store whether the class has a default
    static constexpr bool hasDefault=
      HasDefault;
    
    /// Check if the value is default or not
    bool isDefault()
      const
    {
      // If reference is a serializable class (map), check itself
      if constexpr(isMap)
	{
	  /// Returned value
	  bool is=
	    true;
	  
	  forEach(value.serializableMembers(),
	    [&is](auto s)
	    {
	      is&=
		s.isDefault();
	    });
	  
	  return
	    is;
	}
      else
	// If object has no default, false
	if constexpr(not hasDefault)
	  return
	    false;
	else
	  // If reference is not a serializable class, check default
	  return
	    value==this->def;
    }
    
    /// Returns a YAML node
    YAML::Node serialize()
      const
    {
      /// Returned node
      YAML::Node node;
      
      node[name]=
	value;
      
      return
	node;
    }
    
    /// Copy assignment operator
    Serializable& operator=(const Serializable& oth)
    {
      value=
 	oth.value;
	
	return
	  *this;
    }
  };
  
  DEFINE_HAS_MEMBER(serialize);
  
  /// Stream to an output
  template <typename S,
	    typename T,
	    SFINAE_ON_TEMPLATE_ARG(not canPrint<S,T>),
	    SFINAE_ON_TEMPLATE_ARG(hasMember_serialize<T>)>
  S& operator<<(S&& stream,
		const T& ser)
  {
    /// Converter to string
    YAML::Emitter emitter;
    emitter<<
      ser.serialize();
    
    stream<<
      emitter.c_str();
    
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
      // node>>rhs.a;
      
      return true;
    }
  };
}

#endif
