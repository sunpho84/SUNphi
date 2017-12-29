#ifndef _TRANSPOSE_HPP
#define _TRANSPOSE_HPP

/// \file Transpose.hpp
///
/// \brief Defines a class which take the transposed of a TEx

#include <tens/TensKind.hpp>
#include <tex/BaseTEx.hpp>
#include <tex/Reference.hpp>

namespace SUNphi
{
  /// Class to take the transposed of a TEx
  template <typename A,                                  // Type to transposed
	    typename TK=typename RemoveReference<A>::Tk, // Tens Kind of the bound type
	    typename TK_TYPES=typename TK::types>        // Types of the tensor kind
  class Transposer :
    public TEx<Transposer<A>>,                   // Inherit from TEx to qualify as a TEx
    public ConstrainIsTEx<A>,                    // Constrain B to be a TEx
    public ConstrainIsTensKind<TK>               // Constrain type TK to be a TensKind
  {
    /// Reference of the type to transpose
    Reference<T> ref;
    
  };
}

#endif
