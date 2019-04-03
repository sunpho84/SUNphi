#ifndef _CONTAINERS_MAP_HPP
#define _CONTAINERS_MAP_HPP

/// \file containers/Map.hpp
///
/// \brief Provides a map with char key
///
/// Be careful, use it only with static key! The key is actually a
/// const char*: the pointing string is not copied, so it must be
/// accessible for the whole time needed

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <cstring>
#include <map>

namespace SUNphi
{
  /// Object incapsulating the comparison operator between two c strings
  struct CStringComparer
  {
    /// Comparison operator
    bool operator()(const char* first,  ///< First element to compare
		    const char* second) ///< Second element to compare
      const
    {
      return
	strcmp(first,second)<0;
    }
  };
  
  /// A map which can be safely used with a c-string
  template <typename T> // Mapped type
  using MapWithCStrKey=
    std::map<const char*,T,CStringComparer>;
}


#endif
