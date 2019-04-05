#ifndef _SERIALIZE_SCALAR_HPP
#define _SERIALIZE_SCALAR_HPP

/// \file serialize/Scalar.hpp
///
/// \brief Define a serializable scalar

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

namespace SUNphi
{
  /// Defines a serializable scalar with an explicit tag
#define SERIALIZABLE_SCALAR_WITH_TAG(TYPE,NAME,TAG,...)	\
  Serializable<TYPE> NAME{TAG,__VA_ARGS__}
  
  /// Defines a serializable scalar with the same tag of the variable name
#define SERIALIZABLE_SCALAR(TYPE,NAME,...)	\
  SERIALIZABLE_SCALAR_WITH_TAG(TYPE,NAME,#NAME,__VA_ARGS__)
}

#endif
