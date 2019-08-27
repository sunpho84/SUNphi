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
    /// Fundamental type
    using T=
      decltype((*static_cast<S*>(nullptr))[0]);
    
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
	static_cast<const S&>(*this)==def;
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
      : S(def...)
      , name(name)
      , def(def...)
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
	    forEach(static_cast<const S&>(*this),[&](auto&& s)
		    {
		      if constexpr(hasMember_serialize<S>)
			subNode.push_back(s.serialize(onlyNonDefault));
		      else
			subNode.push_back(s);
		    });
	  else
	    for(auto& s: static_cast<const S&>(*this))
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
	(*this)()=
	  node[name].template as<S>();
      
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
  
  /// Create a serializable sequence with a given tag
#define SERIALIZABLE_SEQUENCE_WITH_TAG(CONTAINER,			\
				       NAME,				\
				       TAG,				\
				       ...)				\
  SerializableSequence<CONTAINER>					\
     NAME{TAG,__VA_ARGS__}
  
  /// Create a serializable sequence
#define SERIALIZABLE_SEQUENCE(CONTAINER,				\
			      NAME,					\
			      ...)					\
  SERIALIZABLE_SEQUENCE_WITH_TAG(CONTAINER,NAME,#NAME,__VA_ARGS__)
  
  /// Create a serializable vector with a given tag
#define SERIALIZABLE_VECTOR_WITH_TAG(TYPE,				\
				     NAME,				\
				     TAG,				\
				     ...)				\
  SERIALIZABLE_SEQUENCE_WITH_TAG(std::vector<TYPE>,NAME,TAG,__VA_ARGS__)
  
  /// Create a serializable vector
#define SERIALIZABLE_VECTOR(TYPE,					\
			    NAME,					\
			    ...)					\
  SERIALIZABLE_VECTOR_WITH_TAG(TYPE,NAME,#NAME,__VA_ARGS__)
  
  /// Create a serializable pair with a given tag
#define SERIALIZABLE_PAIR_WITH_TAG(TYPE1,				\
				   TYPE2,				\
				   NAME,				\
				   TAG,					\
				   ...)					\
  SerializableSequence<std::pair<TYPE1,TYPE2>>				\
    NAME{TAG,__VA_ARGS__}
  
  /// Create a serializable pair
#define SERIALIZABLE_PAIR(TYPE1,					\
			  TYPE2,					\
			  NAME,						\
			  ...)						\
  SERIALIZABLE_PAIR_WITH_TAG(TYPE1,TYPE2,NAME,#NAME,__VA_ARGS__)
  
  DEFINE_IS_THE_TEMPLATED_CLASS(SerializableSequence);
}

#endif
