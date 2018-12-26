#ifndef _WRAP_HPP
#define _WRAP_HPP

/// \file Wrap.hpp
///
/// \brief Header file for the definition of \c Wrapper
///
/// Blocks any kind of simplification of the bound \c SmET: a wrapped
/// smet is not mergeable, vectorizable, and no pattern recognition
/// works on nested smet

#include <smet/NnarySmET.hpp>
#include <smet/Transpose.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  // Base type to qualify as \c Wrapper
  DEFINE_BASE_TYPE(Wrapper);
  
  /// Class to wrap a \c SmET
  template <typename..._Refs>                               // Type of the expression to wrap
  class Wrapper :
    public BaseWrapper,                                     // Inherit from \c BaseWrapper to detect in expression
    public NnarySmET<Wrapper<_Refs...>>,                    // Inherit from \c NnarySmET
    public ConstrainAreSmETs<_Refs...>                      // Constrain all \c _Refs to be \ SmET
  {
  public:
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(1);
    
    IDENTITY_REPRESENTATIVE_FUNCTION;
    
    PROVIDE_SIMPLE_NNARY_COMP_SIZE;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    SAME_TK_AS_REF(0);
    
    PROVIDE_EXTRA_MERGE_DELIMS(IntsUpTo<Tk::nTypes+1>);
    
    REPRESENTATIVE_FUNCTION_WINS_ALL;
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(Wrapper);
  };
  
  // Check that a test \c Wrapper is a \c NnarySmET
  STATIC_ASSERT_IS_NNARY_SMET(Wrapper<Tens<TensKind<TensComp<double,3>>,double>>);
  
  // Build \c Wrapper from \c wrap
  SIMPLE_NNARY_SMET_BUILDER(wrap,Wrapper);
  
  // Simplifies wrap(wrap)
  ABSORB_DUPLICATED_NNARY_SMET_CALL(wrap,Wrapper);
}

#endif
