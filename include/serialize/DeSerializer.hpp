#ifndef _DESERIALIZER_HPP
#define _DESERIALIZER_HPP

/// \file serialize/DeSerializer.hpp
///
/// \brief Used to deserialize quantities
///
/// Incapsulates yaml access providing an input stream operator

#include <yaml-cpp/yaml.h>

#include <debug/Crash.hpp>
#include <ios/Logger.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <serialize/Scalar.hpp>
#include <serialize/Sequence.hpp>

namespace SUNphi
{
  /// Convert to string the passed objects
  template <bool Ref=false>
  class DeSerializer
  {
    /// Node used to store the serialized data
    RefIf<Ref,const YAML::Node> node;
    
    /// Reads everything or the default only
    const bool onlyNonDef{false};
    
  public:
    
    /// Subscript operator
    template <typename T>
    auto operator[](T&& t)
      const
    {
      return
	DeSerializer<true>(node[forw<T>(t)],onlyNonDef);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(operator[]);
    
    /// Input a non-serializable object
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(not (isSerializableClass<T> or isSerializableScalar<T> or isSerializableSequence<T>))>
    friend const DeSerializer& operator>>(const DeSerializer& deSer,
					  T& t)
    {
      try
	{
	  t=
	    deSer.node.template as<T>();
	}
      catch(...)
	{
	  CRASH<<"Unable to convert "<<deSer.node.template as<std::string>();
	}
      
      return
	deSer;
    }
    
    /// Input a serializable scalar
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableScalar<T>)>
    friend const DeSerializer& operator>>(const DeSerializer& deSer,
					  T& t)
    {
      /// Returned value of the query of t
      const auto rc=
	deSer.node[t.name];
      
      if(rc)
	{
	  DeSerializer<true> nestedDeSer(rc,deSer.onlyNonDef);
	  
	  nestedDeSer>>
	    t();
	}
      else
	if constexpr(not T::hasDefault)
	  if(not deSer.onlyNonDef)
	    CRASH<<"Searched serializable scalar "<<t.name<<" not found";
      
      return
	deSer;
    }
    
    /// Input a serializable sequence
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableSequence<T>)>
    friend const DeSerializer& operator>>(const DeSerializer& deSer,
					  T& t)
    {
      /// Returned value of the query of t
      const auto rc=
	deSer.node[t.name];
      
      if(rc)
	{
	  DeSerializer<true> nestedDeSer(rc,deSer.onlyNonDef);
	  
	  /// Number of elements
	  const size_t size=
	    rc.size();
	  
	  t.resize(size);
	  
	  for(size_t i=0;i<size;i++)
	    nestedDeSer[i]>>t[i];
	}
      else
	if constexpr(not T::hasDefault)// bisogna aggiungere un default, che ci sia solo se sono passati parametri di default
	  if(not deSer.onlyNonDef)
	    CRASH<<"Searched serializable sequence "<<t.name<<" not found";
      
      return
	deSer;
    }
    
    /// Input a serializable class
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isSerializableClass<T>)>
    friend const DeSerializer& operator>>(const DeSerializer& deSer,
					  T& t)
    {
      forEach(t.serializableMembers(),
	      [&deSer](auto& s)
	      {
		deSer>>
		  s;
	      });
      
      return
	deSer;
    }
    
    /// Creates a serializer
    DeSerializer(const std::string& str,
		 const bool& onlyNonDef=false)
      : node(YAML::Load(str)),
	onlyNonDef(onlyNonDef)
    {
    }
    
    /// Creates a reference serializer
    DeSerializer(const YAML::Node& node,
		 const bool& onlyNonDef=false)
      : node(node),
	onlyNonDef(onlyNonDef)
    {
    }
  };
}

#endif
