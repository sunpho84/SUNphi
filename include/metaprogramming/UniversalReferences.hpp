#ifndef _UNIVERSALREFERENCE_HPP
#define _UNIVERSALREFERENCE_HPP

/// \file UniversalReferences.hpp
///
/// \brief Defines convenient macro and types for Universal References
///
/// https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// See http://ericniebler.com/2013/08/07/universal-references-and-the-copy-constructo/
#define UNIVERSAL_REFERENCE_CONSTRUCTOR_OF_WRAPPER_DISABLE(T,WRAPPER)	\
  typename=EnableIf<not isBaseOf<WRAPPER<T>,T>>
  
  /////////////////////////////////////////////////////////////////////
  
  /// Forward according to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2951.html
  template <class T,
	    class U,
	    class=EnableIf<(isLvalue<T> ?
			    isLvalue<U> :
			    true) and
			   std::is_convertible <RemoveReference<U>*,
						RemoveReference<T>*>::value>>
  constexpr T&& forw(U&& u)
  {
    return static_cast<T&&>(u);
  }
  
  /////////////////////////////////////////////////////////////////
  
  /// Short name for decltype(auto)
#define DECLAUTO decltype(auto)
}

#endif
