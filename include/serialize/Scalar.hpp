#ifndef _SERIALIZE_SCALAR_HPP
#define _SERIALIZE_SCALAR_HPP

/// \file serialize/Scalar.hpp
///
/// \brief Define a serializable scalar

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

#include <serialize/Base.hpp>
#include <serialize/Binarize.hpp>

namespace SUNphi
{
  /// Marks that the value has no default value
  class NoDefault
  {
  };
  
  /// Used to mark that no default value is provided
  [[ maybe_unused ]]
  static constexpr NoDefault NO_DEFAULT;
  
  /// Used to mark the mandatoriety of the variable
  [[ maybe_unused ]]
  static constexpr NoDefault MANDATORY;
  
  /// Provide a default value for a serializable reference
  template <typename T>
  class SerializableDefaultValue
  {
  public:
    
    /// Default value stored
    const T def;
    
    /// Store the default value
    SerializableDefaultValue<T>(const T& def)
      : def(def)
    {
      static_assert(not isSerializableClass<T>,"A serializable class has already its own defaults");
    }
  };
  
  /// Provide no default value for a serializable reference
  template <>
  class SerializableDefaultValue<NoDefault>
  {
  public:
    
    /// Construct doing nothing
    SerializableDefaultValue<NoDefault>(const NoDefault&)
    {
    }
  };
  
  /// Class wrapping a class to provide scalar node for the serializer
  ///
  /// Provides name and default value
  template <typename T,
	    typename TDef=NoDefault>
  class SerializableScalar :
    public SerializableDefaultValue<TDef>,
    public Binarizable<SerializableScalar<T,TDef>>
  {
    /// Stored variable
    T value;
    
  public:
    
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
    
    /// Creates a serializable scalar with default value
    SerializableScalar(const char* name,
		       const TDef& def)
      :
      SerializableDefaultValue<TDef>(def),
      name(name)
    {
      static_assert((not isSerializableClass<T>) or isSame<RemoveCV<TDef>,NoDefault>,"A serializable class has his own default members");
      
      /// If the variable has default value, copy it
      if constexpr(not isSame<RemoveCV<TDef>,NoDefault>)
	value=def;
    }
    
    /// Store whether the class has a default
    static constexpr bool hasDefault=
      not isSame<TDef,NoDefault>;
    
    /// Check if the value is default or not
    bool isDefault()
      const
    {
      // If reference is a serializable class, check itself
      if constexpr(isSerializableClass<T>)
	return
	  value.isDefault();
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
    
    /// Binarize a SerializableScalar
    friend Binarizer& operator<<(Binarizer& out,                   ///< Output
				 const SerializableScalar& in)     ///< Input
    {
      return
	out<<in();
    }
    
    /// DeBinarize a SerializableScalar
    friend Binarizer& operator>>(Binarizer& in,                    ///< Input
				 SerializableScalar& out)          ///< Output
    {
      return
	in>>out();
    }
    
    /// Used to overload assignment operators
#define TRIVIAL_ASSIGN_OVERLOAD(OP)			\
    /*! Provides the operator OP */			\
    template <typename O> /* Other operand typename */	\
    DECLAUTO operator OP (O&& oth)			\
    {							\
      return value OP					\
 	oth;						\
     }
    
    TRIVIAL_ASSIGN_OVERLOAD(=);
    TRIVIAL_ASSIGN_OVERLOAD(+=);
    TRIVIAL_ASSIGN_OVERLOAD(-=);
    TRIVIAL_ASSIGN_OVERLOAD(*=);
    TRIVIAL_ASSIGN_OVERLOAD(/=);
    
#undef TRIVIAL_ASSIGN_OVERLOAD
  };
  
  /// Create a serializable scalar
#define SERIALIZABLE_SCALAR(TYPE,					\
			    NAME,					\
			    DEFAULT)					\
  SerializableScalar<TYPE,decltype(DEFAULT)>				\
     NAME{#NAME,DEFAULT}
  
  DEFINE_IS_THE_TEMPLATED_CLASS(SerializableScalar);
}

#endif
