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
    
    /// Reference of the type to transpose
    Reference<T> ref;
    
    // Attributes
    NOT_STORING;
    ASSIGNABLE_ACCORDING_TO_REF;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    using Tk=typename TK::Twinned;
    
    // Implement the evaluator
    PROVIDE_STRAIGHT_EVALUATOR(Transposer);
    
    /// Constructor taking a universal reference
    Transposer(T&& ref) : ref(ref)
    {
    }
  };
  
  // Check that a test Transposer is a UnaryTEx
  STATIC_ASSERT_IS_UNARY_TEX(Transposer<Tens<TensKind<TensComp<double,1>>,double>>);
  
  // Build Transposer from transpose
  SIMPLE_UNARY_TEX_BUILDER(transpose,Transposer);
  
  // Simplifies transpose(transpose)
  CANCEL_DUPLICATED_UNARY_TEX_CALL(transpose,Transposer);
}

#endif
