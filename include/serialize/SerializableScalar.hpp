#ifndef _SERIALIZABLE_SCALAR_HPP
#define _SERIALIZABLE_SCALAR_HPP

/// \file SerializableScalar.hpp
///
/// \brief Define a serializable scalar

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Marks that the value has no default value
  class NoDefault
  {
  };
  
  /// Used to mark the mandatoriety of the variable
  [[ maybe_unused ]]
  static constexpr NoDefault NO_DEFAULT;
  
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
	    typename Tdef=NoDefault>
  class SerializableScalar :
    public SerializableDefaultValue<Tdef>
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
		       const Tdef& def)
      :
      SerializableDefaultValue<Tdef>(def),
      name(name)
    {
      /// If the variable has default value, copy it
      if constexpr(not isSame<RemoveCV<Tdef>,NoDefault>)
	value=def;
    }
    
    /// Check if the value is default or not
    bool isDefault()
      const
    {
      if constexpr(isSame<Tdef,NoDefault>)
	return
	  false;
      else
	return
	  value==this->def;
    }
    
    /// Used to overload assignment operators
#define TRIVIAL_ASSIGN_OVERLOAD(OP)			\
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
  
  /// Check if the class is a SerializableScalar
  ///
  /// False case
  template <typename T>
  constexpr bool _isSerializableScalar(const T*)
  {
    return
      false;
  }
  
  /// Check if the class is a SerializableScalar
  ///
  /// True case
  template <typename T,
	    typename Tdef>
  constexpr bool _isSerializableScalar(const SerializableScalar<T,Tdef>*)
  {
    return
      true;
  }
  
  /// Check if the class is a SerializableScalar
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isSerializableScalar=
    _isSerializableScalar((T*)nullptr);
}

#endif
