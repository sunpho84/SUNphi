#ifndef _SERIALIZE_HPP
#define _SERIALIZE_HPP

/// \file Serialize.hpp
///
/// \brief Serializer tool for classes and plain types
///
/// A serializable object is a class which wraps another object, or a
/// plain type, providing a name and a defult value for the object.

#include <ios/Logger.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  DEFINE_BASE_TYPE(Serializable);
  
  /// Serializable class
  template <typename T>      // Wrapped type
  class Serializable :
    public BaseSerializable
  {
    /// Current value
    T value;
    
  public:
    
    /// Default value
    const T def;
    
    /// Name to be used
    const char* name;
    
    const T& operator()()
      const
    {
      return
	value;
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(operator());
    
    /// Cast to base value
    operator T()
    {
      return
	value;
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
    
    /// Construct the serializable
    Serializable(const char* name, ///< Name of the serializable object
		 const T& def)     ///< Default value
      : value(def)
      , def(def)
      , name(name)
    {
    }
    
    // /// Returns the size of the wrapped object, when binarized
    // int binSize()
    //   const
    // {
    //   if constexpr(isClass<T>)
    // 	return
    // 	  value.binSize();
    //   else
    // 	return
    // 	  sizeof(T);
    // }
    
    // /// Write to an array
    // void binarize(char* ptr,
    // 		  int residSize)
    //   const
    // {
    //   if(residSize<binSize())
    // 	CRASH<<"Residual size "<<residSize<<" too small to accommodate "<<binSize();
      
    // }
  };
}

#endif
