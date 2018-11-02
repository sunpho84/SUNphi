#ifndef _TRANSPOSE_HPP
#define _TRANSPOSE_HPP

/// \file Transpose.hpp
///
/// \brief Defines a class which take the transposed of a SmET

#include <tens/TensClass.hpp>
#include <smet/Reference.hpp>
#include <smet/UnarySmET.hpp>

namespace SUNphi
{
  // Base type to qualify as Transposer
  DEFINE_BASE_TYPE(Transposer);
  
  /// Class to take the transposed of a SmET
  template <typename _Ref,                                  // Type to be transposed
	    typename TK=typename RemRef<_Ref>::Tk> // Tens Kind of the bound type
  class Transposer :
    public BaseTransposer,                         // Inherit from BaseTransposer to detect in expression
    public UnarySmET<Transposer<_Ref>>,            // Inherit from UnarySmET
    public ConstrainIsSmET<_Ref>,                  // Constrain Ref to be a SmET
    public ConstrainIsTensKind<TK>                 // Constrain type TK to be a TensKind
  {
    
  public:
    
    /// Returns the size of a component
    ///
    /// The result is the size of the twinned component
    template <typename TC>
    int compSize() const
    {
      return ref.template compSize<TwinCompOf<TC>>();
    }
    
    PROVIDE_UNARY_SMET_REF;
    
    // Attributes
    NOT_STORING;
    ASSIGNABLE_ACCORDING_TO_REF;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    PROVIDE_TK(typename TK::Twinned);
    
    SAME_FUND_TYPE_OF_REF;
    
    PROVIDE_MERGEABLE_COMPS(/* We have to split at all true twinned components */,
			    InsertTrueTwinnedPosOfTuple<
			       typename Unqualified<Ref>::MergeableComps, // Nested split points
			       typename Tk::types>);                      // Tuple with components
    
    PROVIDE_UNARY_SMET_SIMPLE_GET_MERGED_COMPS_VIEW(Transposer);
    PROVIDE_UNARY_SMET_ASSIGNEMENT_OPERATOR(Transposer);
    PROVIDE_UNARY_SMET_DEFAULT_EVALUATOR;
    PROVIDE_UNARY_SMET_SIMPLE_CREATOR(Transposer);
  };
  
  // Check that a test Transposer is a UnarySmET
  STATIC_ASSERT_IS_UNARY_SMET(Transposer<Tens<TensKind<TensComp<double,1>>,double>>);
  
  // Build Transposer from transpose
  SIMPLE_UNARY_SMET_BUILDER(transpose,Transposer);
  
  // Simplifies transpose(transpose)
  CANCEL_DUPLICATED_UNARY_SMET_CALL(transpose,Transposer);
  
  // Move Transposer to the lhs
  UNARY_SMET_GOES_ON_LHS(transpose,Transposer);
}

#endif
