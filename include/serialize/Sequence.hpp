#ifndef _SERIALIZE_SEQUENCE_HPP
#define _SERIALIZE_SEQUENCE_HPP

/// \file serialize/Sequence.hpp
///
/// \brief Container mapping a sequence
///
/// A Serializable container contains a begin, end and size methods,
/// as an STL container

#include <cstddef>

#include <metaprogramming/TypeTraits.hpp>
#include <serialize/Binarize.hpp>
#include <serialize/Default.hpp>

namespace SUNphi
{
  /// Class providing sequence node for the serializer
  ///
  /// Provides name and default value
  template <typename S>
  class SerializableSequence
    : public S
    , public Binarizable<SerializableSequence<S>>
  {
    /// Fundamental type
    using T=
      decltype(S{}[0]);
    
  public:
    
    /// Name used to refer
    const char* name;
    
    /// Default value
    const S def;
    
    /// Returns whether the container is default
    bool isDefault()
      const
    {
      return
	static_cast<S>(*this)==def;
    }
    
    /// Creates a serializable vector with name
    template <typename...TDef>
    SerializableSequence(const char* name,      ///< Name of the sequence
			 TDef&&...def)          ///< Initializer
      : S(forw<TDef>(def)...)
      , name(name)
      , def(forw<TDef>(def)...)
    {
    }
    
    /// Access the value
    const S& operator()()
      const
    {
      return
	static_cast<const S&>(*this);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(operator());
    
    /// Copy assignment operator
    SerializableSequence& operator=(const SerializableSequence& oth)
    {
      (*this)()=
 	(oth)();
	
	return
	  *this;
    }
  };
  
  /// Create a serializable sequence
#define SERIALIZABLE_SEQUENCE(CONTAINER,				\
			      NAME,					\
			      ...)					\
  SerializableSequence<CONTAINER>					\
     NAME{#NAME,__VA_ARGS__}
  
  /// Create a serializable vector
#define SERIALIZABLE_VECTOR(TYPE,					\
			    NAME,					\
			    ...)					\
  SERIALIZABLE_SEQUENCE(std::vector<TYPE>,NAME,__VA_ARGS__)
  
  DEFINE_IS_THE_TEMPLATED_CLASS(SerializableSequence);
}

#endif
