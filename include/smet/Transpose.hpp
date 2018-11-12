#ifndef _TRANSPOSE_HPP
#define _TRANSPOSE_HPP

/// \file Transpose.hpp
///
/// \brief Defines a class which take the transposed of a SmET

#include <tens/TensClass.hpp>
#include <smet/Reference.hpp>
#include <smet/NnarySmET.hpp>

namespace SUNphi
{
  // Base type to qualify as \c Transposer
  DEFINE_BASE_TYPE(Transposer);
  
  /// Class to take the transposed of a \c SmET
  template <typename..._Refs>                      // Type to be transposed
  class Transposer :
    public BaseTransposer,                         // Inherit from \c BaseTransposer to detect in expression
    public NnarySmET<Transposer<_Refs...>>,        // Inherit from \c NnarySmET
    public ConstrainAreSmETs<_Refs...>             // Constrain all \c Refs to be \c SmET
  {
    
  public:
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(1);
    
    IDENTITY_REPRESENTATIVE_FUNCTION;
    
    /// Returns the size of a component
    ///
    /// The result is the size of the twinned component
    template <typename TC>
    int compSize() const
    {
      return get<0>(refs).template compSize<TwinCompOf<TC>>();
    }
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    /// TensorKind of the bound expression
    PROVIDE_TK(typename TkOf<Ref<0>>::Twinned);
    
    NO_EXTRA_MERGE_DELIMS;
    
    REPRESENTATIVE_FUNCTION_WINS_ALL;
    
    PROVIDE_UNARY_SMET_ASSIGNEMENT_OPERATOR(Transposer);
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(Transposer);
  };
  
  // Check that a test \c Transposer is a \c NnarySmET
  STATIC_ASSERT_IS_NNARY_SMET(Transposer<Tens<TensKind<TensComp<double,1>>,double>>);
  
  // Build Transposer from \c transpose
  SIMPLE_NNARY_SMET_BUILDER(transpose,Transposer);
  
  // Simplifies transpose(transpose)
  CANCEL_DUPLICATED_NNARY_SMET_CALL(transpose,Transposer);
  
  // Move \c Transposer to the lhs
  UNARY_SMET_GOES_ON_LHS(transpose,Transposer);
}

#endif
