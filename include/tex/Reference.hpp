#ifndef _REFERENCE_HPP
#define _REFERENCE_HPP

/// \file Reference.hpp
///
/// \brief Defines a class which defines a reference to another TEx
///
/// \todo This should be the place where the instantiation depending
/// on the cost of the expression is made

#include <tex/BaseTEx.hpp>

namespace SUNphi
{
  /// Reference to T or new type, depending on wheter the class isStoring
  template <typename T,                 // Type to get reference from
	    typename=ConstrainIsTEx<T>> // Constrain the type to be a TEx
  using Reference=Conditional<RemoveReference<T>::isStoring,T&,T>;
}

#endif
