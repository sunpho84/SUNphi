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
  /// Defines a serializable scalar
#define SERIALIZABLE_SCALAR(TYPE,NAME,...)	\
  Serializable<TYPE> NAME{#NAME,__VA_ARGS__}
}

#endif
