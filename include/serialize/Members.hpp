#ifndef _SERIALIZABLEMEMBERS_HPP
#define _SERIALIZABLEMEMBERS_HPP

/// \file serialize/Members.hpp
///
/// \brief Creates a list of a set of serializable scalars
///
/// The list is a tuple, which can be added as a member to a class, to
/// make the class itself serializable

#include <serialize/Base.hpp>
#include <serialize/Scalar.hpp>
#include <tuple/TupleClass.hpp>

namespace SUNphi
{
  /// Helper function to create the serializable members
  template <typename...Ts>
  auto _serializableList(Ts&&...ts)
  {
    /// Check that all arguments are SerializableScalar
    static_assert((isSerializableScalar<Ts> && ...),"All arguments need to be SerializableScalar");
    
    return
      std::forward_as_tuple(ts...);
  }
  
  /// Helper function to determine if all passed value have a default
  template <typename...Ts>
  static constexpr bool _hasDefault(const Tuple<Ts...>*)
  {
    return
      (RemRef<Ts>::hasDefault && ...);
  }
  
  /// Defines a list of serializable members
#define SERIALIZABLE_MEMBERS(...)					\
  decltype(_serializableList(__VA_ARGS__))				\
  serializableMembers{_serializableList(__VA_ARGS__)};			\
									\
  /*! Determine whether all members had a default value*/		\
  static constexpr bool hasDefault=					\
    _hasDefault((RemRef<decltype(serializableMembers)>*)nullptr);	\
  									\
  /*! Iterates on all elements checking defaultness */			\
  bool const isDefault()						\
    const								\
  {									\
    /*! Returned value */						\
    bool is=								\
      true;								\
    									\
    forEach(this->serializableMembers,					\
	    [&is](auto s)						\
	    {								\
	      is&=							\
		s.isDefault();						\
	    });								\
    									\
    return								\
      is;								\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
}

#endif
