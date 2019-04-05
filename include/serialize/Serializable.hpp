#ifndef _SERIALIZE_SERIALIZABLE_HPP
#define _SERIALIZE_SERIALIZABLE_HPP

/// \file serialize/Serializable.hpp
///
/// \brief Define a serializable quantity

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <yaml-cpp/yaml.h>

#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>
#include <serialize/Base.hpp>
#include <serialize/Binarize.hpp>
#include <serialize/Default.hpp>
#include <utility/Macros.hpp>

namespace SUNphi
{
  /// Provides a class with serializability features via CRTP
  ///
  /// The type can be only trivial, or a serializable class (map). A
  /// dedicated wrapper is in place for a sequence
  template <typename T>
  class Serializable
    : public SerializableDefaultValue<T,not isSerializableMap<T>>
    , public Binarizable<Serializable<T>>
  {
    /// Detect if the value is a map
    static constexpr bool isMap=
      isSerializableMap<T>;
    
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
    explicit Serializable(const char* name,    ///< Name to be used
			  const T& def={})     ///< Default, if passed
      :
      SerializableDefaultValue<T,not isMap>(def),
      name(name)
    {
      if constexpr(not isMap)
	this->putToDefault();
    }
    
    /// Check if the value is default or not
    bool isDefault()
      const
    {
      // If the value can be checked, check it
      if constexpr(hasMember_isDefault<T>)
	return
	  value.isDefault();
      else
	// If reference is not a serializable class, check default
	return
	  value==this->def;
    }
    
    /// Returns a YAML node after serializing to it
    YAML::Node serialize(YAML::Node& node,
			 const bool& onlyNonDefault=false)
      const
    {
      if(not (onlyNonDefault and isDefault()))
	{
	  if constexpr(hasMember_serialize<T>)
	    node[name]=
	      value.serialize(onlyNonDefault);
	  else
	    node[name]=
	      value;
	}
      
      return
	node;
    }
    
    /// Returns a YAML node
    YAML::Node serialize(const bool& onlyNonDefault=false)
      const
    {
      /// Returned value
      YAML::Node node;
      
      return
	serialize(node,onlyNonDefault);
    }
    
    /// Convert from a YAML node
    bool deSerialize(const YAML::Node& node) ///< Node from which to convert
    {
      if(not node[name])
	this->putToDefault();
      else
	{
	  value=
	    node[name].template as<T>();
	}
      
      return
	true;
    }
    
    /// Provide a simple friend binary operator
#define PROVIDE_SIMPLE_FRIEND_BINARY_OPERATOR(OP)	\
    /*! Assignment operator               */		\
    template <typename O>				\
    DECLAUTO friend operator OP (O& first,			\
				 const Serializable& second)	\
    {							\
      return						\
	first OP second.value;				\
    }							\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    /// Provide a simple binary operator
#define PROVIDE_SIMPLE_BINARY_OPERATOR(OP)			\
    /*! Assignment operator               */			\
    template <typename O,					\
	      SFINAE_ON_TEMPLATE_ARG(not isSame<O,Serializable>)>	\
    DECLAUTO operator OP (const O& oth)				\
    {								\
      return							\
	value OP oth;						\
    }								\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_ALL_BINARY_OPERATORS(PROVIDE_SIMPLE_BINARY_OPERATOR);
    PROVIDE_ALL_BINARY_OPERATORS(PROVIDE_SIMPLE_FRIEND_BINARY_OPERATOR);
    
    /// Assignment operator
    ///
    /// Need to stay on his own
    template <typename O>
    DECLAUTO operator=(const O& oth)
    {
      return
	value=oth;
    }
    
#undef PROVIDE_SIMPLE_BINARY_OPERATOR
    
    /// Copy assignment operator
    Serializable& operator=(const Serializable& oth)
    {
      value=
 	oth.value;
	
	return
	  *this;
    }
    
    /// Cast to base value
    operator T&()
    {
      return
	value;
    }
    
    /// Cast to base value, const version
    operator const T&()
      const
    {
      return
	value;
    }
  };
  
  DEFINE_IS_THE_TEMPLATED_CLASS(Serializable);
}

#endif
