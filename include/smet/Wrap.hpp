#ifndef _WRAP_HPP
#define _WRAP_HPP

/// \file Wrap.hpp
///
/// \brief Header file for the definition of wrapper

#include <tens/TensKind.hpp>
#include <smet/Transpose.hpp>
#include <smet/UnarySmET.hpp>

namespace SUNphi
{
  // Base type to qualify as Wrapper
  DEFINE_BASE_TYPE(Wrapper);
  
  /// Class to wrap a SmET
  template <typename _Ref,                                  // Type of the expression to wrap
	    typename TK=typename RemoveReference<_Ref>::Tk> // Tens Kind of the wrapped expression
  class Wrapper :
    public BaseWrapper,                                // Inherit from BaseWrapper to detect in expression
    public UnarySmET<Wrapper<_Ref>>,                    // Inherit from UnarySmET
    public ConstrainIsSmET<_Ref>,                       // Constrain _Ref to be a SmET
    public ConstrainIsTensKind<TK>                     // Constrain type TK to be a TensKind
  {
  public:
    
    PROVIDE_UNARY_SMET_REF;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    using Tk=TK;
    
    SAME_COMP_SIZES_OF_REF;
    
    MERGEABLE_ACCORDING_TO_REF;
    PROVIDE_UNARY_SMET_SIMPLE_GET_MERGED_COMPS_VIEW(Wrapper);
    
    PROVIDE_UNARY_SMET_SIMPLE_CREATOR(Wrapper);
    PROVIDE_UNARY_SMET_DEFAULT_EVALUATOR;
  };
  
  // Check that a test Wrapper is a UnarySmET
  STATIC_ASSERT_IS_UNARY_SMET(Wrapper<Tens<TensKind<TensComp<double,3>>,double>>);
  
  // Build Wrapper from wrap
  SIMPLE_UNARY_SMET_BUILDER(wrap,Wrapper);
  
  // Simplifies wrap(wrap)
  ABSORB_DUPLICATED_UNARY_SMET_CALL(wrap,Wrapper);
}

#endif
