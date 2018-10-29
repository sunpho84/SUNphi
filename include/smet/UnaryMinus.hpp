#ifndef _UNARYMINUS_HPP
#define _UNARYMINUS_HPP

/// \file UnaryMinus.hpp
///
/// \brief Header file for the definition of -smet

#include <tens/TensKind.hpp>
#include <physics/Spin.hpp>
#include <smet/UnarySmET.hpp>

namespace SUNphi
{
  // Base type to qualify as UMinuser
  DEFINE_BASE_TYPE(UMinuser);
  
  /// Class to take the conjugate of a SmET
  template <typename _Ref,                                  // Type of the expression to conjugate
	    typename TK=typename RemoveReference<_Ref>::Tk, // Tens Kind of the conjugated type
	    typename TK_TYPES=typename TK::types>           // Types of the tensor kind
  class UMinuser :
    public BaseUMinuser,                                // Inherit from BaseUMinuser to detect in expression
    public UnarySmET<UMinuser<_Ref>>,                   // Inherit from UnarySmET
    public ConstrainIsSmET<_Ref>,                       // Constrain _Ref to be a SmET
    public ConstrainIsTensKind<TK>                      // Constrain type TK to be a TensKind
  {
    
  public:
    
    PROVIDE_UNARY_SMET_REF;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    PROVIDE_TK(TK);
    
    /// Fundamental type
    SAME_FUND_TYPE_OF_REF;
    
    MERGEABLE_ACCORDING_TO_REF;
    
    PROVIDE_UNARY_SMET_SIMPLE_GET_MERGED_COMPS_VIEW(UMinuser);
    
    SAME_COMP_SIZES_OF_REF;
    
    PROVIDE_UNARY_SMET_SIMPLE_CREATOR(UMinuser);
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_EVALUATOR(QUALIFIER)			\
    /*! QUALIFIER evaluator for UMinuser                             */ \
    /*!                                                              */	\
    template <typename...Args>           /* Type of the arguments    */	\
    DECLAUTO eval(const Args&...args)    /*!< Components to get      */	\
      QUALIFIER								\
    {									\
      STATIC_ASSERT_ARE_N_TYPES(TK::nTypes,args);			\
									\
      /*  Temporary result */						\
      const auto val=ref.eval(forw<const Args>(args)...);		\
									\
      return -val;							\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_CONST_OR_NOT_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_CONST_OR_NOT_EVALUATOR
    
  };
  
  // Check that a test UMinuser is a UnarySmET
  STATIC_ASSERT_IS_UNARY_SMET(UMinuser<Tens<TensKind<Spin>,double>>);
  
  // Build UMinuser from uminus
  SIMPLE_UNARY_SMET_BUILDER(uMinus,UMinuser);
  
  // Simplifies uMinus(uMinus)
  CANCEL_DUPLICATED_UNARY_SMET_CALL(uMinus,UMinuser);
  
  /// Implement -smet: return uminus
  template <typename T,              // Type of the expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T>)>
  DECLAUTO operator-(T&& smet)       ///< Expression
  {
    return uMinus(forw<T>(smet));
  }
}

#endif
