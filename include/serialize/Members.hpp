#ifndef _SERIALIZABLEMEMBERS_HPP
#define _SERIALIZABLEMEMBERS_HPP

/// \file serialize/Members.hpp
///
/// \brief Creates a list of a set of serializable scalars
///
/// The list is a tuple, which can be added as a member to a class, to
/// make the class itself serializable

#include <metaprogramming/CRTP.hpp>
#include <serialize/Base.hpp>
#include <serialize/Scalar.hpp>
#include <tuple/TupleClass.hpp>

namespace SUNphi
{
  /// Helper function to create the serializable members
  template <typename...Ts>
  auto _serializableMembers(Ts&&...ts)
  {
    /// Check that all arguments are SerializableScalar
    static_assert((isSerializableScalar<Ts> && ...),"All arguments need to be SerializableScalar");
    
    return
      std::forward_as_tuple(ts...);
  }
  
  template <typename T>
  class SerializableClass
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// 2nd-order helper function to determine if all passed value have a default
    template <typename...Ts>
    static constexpr bool _hasDefault(const Tuple<Ts...>*)
    {
      return
	(RemRef<Ts>::hasDefault && ...);
    }
    
    /// 1st-order helper function to determine if all passed value have a default
    static constexpr bool _hasDefault()
    {
      /// Type of the tuple collecting the referred members
      using Members=
	decltype(static_cast<T*>(nullptr)->serializableMembers());
      
      return
	_hasDefault(static_cast<Members*>(nullptr));
    }
    
  public:
    
    /// Iterates on all elements checking defaultness
    bool const isDefault()
      const
    {
      /// Returned value
      bool is=
	true;
      
      forEach((~(*this)).serializableMembers(),
	    [&is](auto s)
	    {
	      is&=
		s.isDefault();
	    });
      
      return
	is;
    }
    
    /// Iterates on all elements summing their size
    size_t const binSize()
      const
    {
      /// Returned value
      size_t size=
	0;
      
      forEach((~(*this)).serializableMembers(),
	    [&size](auto s)
	    {
	      size+=
		s.binSize();
	    });
      
      return
	size;
    }
    
    /// Determine whether all members had a default value
    static constexpr bool hasDefault=
      _hasDefault();
  };
  
  /// Provides the class embedding with a serializableMemebers method
#define CONST_OR_NOT_SERIALIZABLE_MEMBERS(CONST,			\
					  ...)				\
  /*! Gets the reference to the serializable members */			\
  auto serializableMembers()						\
    CONST								\
  {									\
    return								\
      std::forward_as_tuple(__VA_ARGS__);				\
  } 									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Defines a list of serializable members
#define SERIALIZABLE_MEMBERS(...)					\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(     , __VA_ARGS__);		\
  CONST_OR_NOT_SERIALIZABLE_MEMBERS(const, __VA_ARGS__)
}

#endif
