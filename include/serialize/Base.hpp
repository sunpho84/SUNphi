#ifndef _SERIALIZE_BASE_HPP
#define _SERIALIZE_BASE_HPP

/// \file serialize/Base.hpp
///
/// \brief Defines the basic serializable features

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  // Check existence of serializableMembers member
  DEFINE_HAS_MEMBER(serializableMembers);
  
  /// Check whether the class is serializable
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isSerializableClass=
    hasMember_serializableMembers<T>;
  
  // Check existence of binSize method
  DEFINE_HAS_MEMBER(binSize);
  
  /// Provides an enabler on the specific case isSerializableClass
  PROVIDE_ENABLE_IF_FOR_IS_TYPE(SerializableClass);
}

#endif
