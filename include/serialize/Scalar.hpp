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
#include <serialize/Binarize.hpp>
#include <serialize/Default.hpp>
#include <serialize/Serializable.hpp>

namespace SUNphi
{
  /// Class wrapping a class to provide scalar node for the serializer
  ///
  /// Provides name and default value
  template <typename T,
	    bool HasDefault=true>
  class SerializableScalar :
    public SerializableDefaultValue<T,HasDefault>,
    public Serializable<SerializableScalar<T,HasDefault>>,
    public Binarizable<SerializableScalar<T,HasDefault>>
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
		       const T& def={})
      :
      SerializableDefaultValue<T,HasDefault>(def),
      name(name)
    {
      static_assert((not isSerializableClass<T>) or (not hasDefault),"A serializable class has his own default members");
      
      /// If the variable has default value, copy it
      if constexpr(hasDefault)
	value=this->def;
    }
    
    // SerializableScalar(const char* name,
    // 		       NoDefault)
    //   :
    //   SerializableDefaultValue<T,const NoDefault>(NO_DEFAULT),
    //   name(name)
    // {
    // }
    
    /// Store whether the class has a default
    static constexpr bool hasDefault=
      HasDefault;
    
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
  
  /// Trait to recognize a class as a serializable scalar
  ///
  /// False case
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isSerializableScalar=
    false;
  
  /// Trait to recognize a class as a serializable scalar
  ///
  /// True case
  template <typename T,
	    bool B,
	    template <typename,bool> typename C>
  constexpr bool isSerializableScalar<C<T,B>> =
    true;
}

#endif
