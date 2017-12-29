#ifndef _TRANSPOSE_HPP
#define _TRANSPOSE_HPP

/// \file Transpose.hpp
///
/// \brief Defines a class which take the transposed of a TEx

#include <tens/TensClass.hpp>
#include <tex/Reference.hpp>
#include <tex/UnaryTEx.hpp>

namespace SUNphi
{
  /// Class to take the transposed of a TEx
  template <typename T,                                  // Type to be transposed
	    typename TK=typename RemoveReference<T>::Tk> // Tens Kind of the bound type
  class Transposer :
    public UnaryTEx<Transposer<T>>,              // Inherit from UnaryTEx
    public ConstrainIsTEx<T>,                    // Constrain B to be a TEx
    public ConstrainIsTensKind<TK>               // Constrain type TK to be a TensKind
  {
    
  public:
    
    /// Returns whether this TEx is storing
    static constexpr bool isStoring=false;
    
    /// TensorKind of the bound expression
    using Tk=typename TK::Twinned;
    
    /// Reference of the type to transpose
    Reference<T> ref;
    
    /// Constructor taking a universal reference
    Transposer(T&& ref) : ref(ref)
    {
    }
  };
  
  // Check that a test Transposer is a UnaryTEx
  STATIC_ASSERT_IS_UNARY_TEX(Transposer<Tens<TensKind<TensComp<double,1>>,double>>);
  
}

#endif
