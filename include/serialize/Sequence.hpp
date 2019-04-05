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
#include <tuple/TupleElements.hpp>

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
    /// Helper to determine the mapped type
    static auto _mappedTypeHelper()
    {
      if constexpr(isTupleLike<S>)
	return
	  get<0>(S{});
      else
	return
	  S{}[0];
    }
    
    /// Fundamental type
    using T=
      decltype(_mappedTypeHelper());
    
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
    
    /// Put to default value
    void putToDefault()
    {
      static_cast<S>(*this)=
	def;
    }
    
    /// Creates a serializable vector with name
    template <typename...TDef>
    SerializableSequence(const char* name,      ///< Name of the sequence
			 TDef&&...def)          ///< Initializer
      : S(forw<TDef>(def)...)
      , name(name)
      , def(static_cast<S>(*this))
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
    
    /// Returns a YAML node after serializing to it
    YAML::Node serialize(YAML::Node& node,
			 const bool& onlyNonDefault=false)
      const
    {
      if(not (onlyNonDefault and isDefault()))
	{
	  /// Create the subnode
	  YAML::Node subNode=
	    node[name];
	  
	  if constexpr(isTupleLike<S>)
	    forEach(static_cast<S>(*this),[&](auto&& s)
		    {
		      if constexpr(hasMember_serialize<S>)
			subNode.push_back(s.serialize(onlyNonDefault));
		      else
			subNode.push_back(s);
		    });
	  else
	    for(auto& s: static_cast<S>(*this))
	      {
		if constexpr(hasMember_serialize<S>)
		  subNode.push_back(s.serialize(onlyNonDefault));
		else
		  subNode.push_back(s);
	      }
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
	  static_cast<S>(*this)=
	    node[name].template as<S>();
	}
      
      return
	true;
    }
    
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
  
  /// Create a serializable pair
#define SERIALIZABLE_PAIR(TYPE1,					\
			  TYPE2,					\
			  NAME,						\
			  ...)						\
  SerializableSequence<std::pair<TYPE1,TYPE2>>				\
    NAME{#NAME,__VA_ARGS__}
  
  DEFINE_IS_THE_TEMPLATED_CLASS(SerializableSequence);
}

#endif
