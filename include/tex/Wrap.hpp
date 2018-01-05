#ifndef _WRAP_HPP
#define _WRAP_HPP

/// \file Wrap.hpp
///
/// \brief Header file for the definition of wrapper

#include <physics/Compl.hpp>
#include <tens/TensKind.hpp>
#include <tex/Transpose.hpp>
#include <tex/UnaryTEx.hpp>

namespace SUNphi
{
  /// Class to wrap a TEx
  template <typename W,                                  // Type of the expression to wrap
	    typename TK=typename RemoveReference<W>::Tk> // Tens Kind of the wrapped expression
  class Wrapper :
    public UnaryTEx<Wrapper<W>>,                    // Inherit from UnaryTEx
    public ConstrainIsTEx<W>,                       // Constrain W to be a TEx
    public ConstrainIsTensKind<TK>                  // Constrain type TK to be a TensKind
  {
  public:
    
  /// Reference to the object to wrap
    Reference<W> ref;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    using Tk=TK;
    
    PROVIDE_UNARY_TEX_SIMPLE_CREATOR(Wrapper,W);
    PROVIDE_UNARY_TEX_DEFAULT_EVALUATOR(W);
  };
  
  // Check that a test Wrapper is a UnaryTEx
  STATIC_ASSERT_IS_UNARY_TEX(Wrapper<Tens<TensKind<Compl>,double>>);
  
  // Build Wrapper from wrap
  SIMPLE_UNARY_TEX_BUILDER(wrap,Wrapper);
  
  // Simplifies wrap(wrap)
  ABSORB_DUPLICATED_UNARY_TEX_CALL(wrap,Wrapper);
}

#endif
