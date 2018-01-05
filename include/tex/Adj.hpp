#ifndef _ADJ_HPP
#define _ADJ_HPP

/// \file Adj.hpp
///
/// \brief Defines the adjoint as transposed of the conjugated

#include <tex/Conj.hpp>
#include <tex/Transpose.hpp>

namespace SUNphi
{
  /// Adjoint expression \c ref
  template <typename T>       // Type to adjoint
  DECLAUTO adj(T&& ref)       ///< Quantity to adjoint
  {
    //cout<<"Adjoining type "<<T::name()<<endl;
    return transpose(conj(forw<T>(ref)));
  }
}

#endif