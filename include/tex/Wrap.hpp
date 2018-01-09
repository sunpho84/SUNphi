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
  // Base type to qualify as Wrapper
  DEFINE_BASE_TYPE(Wrapper);
  
  /// Class to wrap a TEx
  template <typename _Ref,                                  // Type of the expression to wrap
	    typename TK=typename RemoveReference<_Ref>::Tk> // Tens Kind of the wrapped expression
  class Wrapper :
    public BaseWrapper,                                // Inherit from BaseWrapper to detect in expression
    public UnaryTEx<Wrapper<_Ref>>,                    // Inherit from UnaryTEx
    public ConstrainIsTEx<_Ref>,                       // Constrain _Ref to be a TEx
    public ConstrainIsTensKind<TK>                     // Constrain type TK to be a TensKind
  {
  public:
    
    PROVIDE_UNARY_TEX_REF;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE; /// \todo check that we want this to happen
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    using Tk=TK;
    
    PROVIDE_UNARY_TEX_SIMPLE_CREATOR(Wrapper);
    PROVIDE_UNARY_TEX_DEFAULT_EVALUATOR(Wrapper);
  };
  
  // Check that a test Wrapper is a UnaryTEx
  STATIC_ASSERT_IS_UNARY_TEX(Wrapper<Tens<TensKind<Compl>,double>>);
  
  // Build Wrapper from wrap
  SIMPLE_UNARY_TEX_BUILDER(wrap,Wrapper);
  
  // Simplifies wrap(wrap)
  ABSORB_DUPLICATED_UNARY_TEX_CALL(wrap,Wrapper);
}

#endif
