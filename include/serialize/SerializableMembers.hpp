#ifndef _SERIALIZABLEMEMBERS_HPP
#define _SERIALIZABLEMEMBERS_HPP

/// \file SerializableMembers.hpp
///
/// \brief Creates a list of a set of serializable scalars
///
/// The list is a tuple, which can be added as a member to a class, to
/// make the class itself serializable

#include <serialize/SerializableScalar.hpp>
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
  
  /// Defines a list of serializable members
#define SERIALIZABLE_MEMBERS(...)					\
  decltype(_serializableList(__VA_ARGS__))				\
  serializableMembers{_serializableList(__VA_ARGS__)}
}

#endif
