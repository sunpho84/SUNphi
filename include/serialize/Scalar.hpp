#ifndef _SERIALIZE_SCALAR_HPP
#define _SERIALIZE_SCALAR_HPP

/// \file serialize/Scalar.hpp
///
/// \brief Define a serializable scalar

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <yaml-cpp/yaml.h>

#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>
#include <serialize/Base.hpp>
#include <serialize/Binarize.hpp>
#include <serialize/Default.hpp>

namespace SUNphi
{
  /// Provides a class with serializability features via CRTP
  template <typename T,
	    bool HasDefault=true,
	    bool isScalar=SUNphi::isSerializableScalar<T>,
	    bool isMap=isSerializableMap<T>,
	    bool isTupleLike=SUNphi::isTupleLike<T>,
	    bool isVectorLike=SUNphi::isVectorLike<T>>
  class Serializable
    : public SerializableDefaultValue<T,HasDefault>
    , public Binarizable<Serializable<T,HasDefault,isScalar,isMap,isTupleLike,isVectorLike>>
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
    explicit Serializable(const char* name, ///< Name to be used
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
}

#endif
