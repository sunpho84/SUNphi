#ifndef _CONJ_HPP
#define _CONJ_HPP

/// \file Conj.hpp
///
/// \brief Header file for the definition of the conjugate of a number

#include <physics/Compl.hpp>
#include <tens/TensKind.hpp>
#include <smet/Transpose.hpp>
#include <smet/UnarySmET.hpp>

namespace SUNphi
{
  // Base type to qualify as Conjer
  DEFINE_BASE_TYPE(Conjer);
  
  /// Class to take the conjugate of a SmET
  template <typename _Ref,                            // Type of the expression to conjugate
	    typename TK=typename RemRef<_Ref>::Tk,    // Tens Kind of the conjugated type
	    typename TK_TYPES=typename TK::types>     // Types of the \c TensKind
  class Conjer :
    public BaseConjer,                                // Inherit from \c BaseConjer to detect in expression
    public UnarySmET<Conjer<_Ref>>,                   // Inherit from \c UnarySmET
    public ConstrainIsSmET<_Ref>,                     // Constrain \c _Ref to be a \c SmET
    public ConstrainIsTensKind<TK>,                   // Constrain type \c TK to be a \c TensKind
    public ConstrainTupleHasType<Compl,TK_TYPES>      // Constrain \c TG to be in the Types of the \c TensKind
  {
    
    /// Position of the Compl component
    constexpr static int posOfCompl=
      posOfType<Compl,TK_TYPES>;
    
  public:
    
    PROVIDE_UNARY_SMET_REF;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    PROVIDE_TK(TK);
    
    SAME_FUND_TYPE_OF_REF;
    
    // We remove at Pos, shift and insert back
    PROVIDE_MERGEABLE_COMPS(/* We have to split at the Compl components, and at the next one */,
			    InsertIntSeqInOrderedIntSeq<
			    IntSeq<posOfCompl,posOfCompl+1>,           // Split at the complex component position and next one
			    typename Unqualified<Ref>::MergeableComps, // Nested components
			    IntSeq<0,0>,                               // Shift 0 after insertion
			    true>);                                    // Ignore if already present
    
    PROVIDE_UNARY_SMET_SIMPLE_GET_MERGED_COMPS_VIEW(Conjer);
    
    SAME_COMP_SIZES_OF_REF;
    
    PROVIDE_UNARY_SMET_SIMPLE_CREATOR(Conjer);
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_EVALUATOR(QUALIFIER)			\
    /*! QUALIFIER evaluator for Conjer                               */	\
    /*!                                                              */	\
    template <typename...Args>           /* Type of the arguments    */	\
    DECLAUTO eval(const Args&...args)    /*!< Components to get      */	\
      QUALIFIER								\
    {									\
      STATIC_ASSERT_ARE_N_TYPES(TK::nTypes,args);			\
									\
      /* Detect if we have to put "-" in the result */			\
      const bool isIm=get<posOfCompl>(Tuple<Args...>(args...));		\
									\
      const int sign=1-isIm*2;						\
									\
      /*  Temporary result */						\
      const auto val=ref.eval(forw<const Args>(args)...);		\
									\
      return sign*val;							\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_CONST_OR_NOT_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_CONST_OR_NOT_EVALUATOR
    
  };
  
  // Check that a test Conjer is a UnarySmET
  STATIC_ASSERT_IS_UNARY_SMET(Conjer<Tens<TensKind<Compl>,double>>);
  
  /// Build Conjer from conj, if TensKind is complex
  template <typename T, 	    // Type of the SmET to get
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  auto conj(T&& smet,               //!< SmET to act upon
	    SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    /// Tens Kind of the conjugated type
    using TK=
      typename RemRef<T>::Tk;
    
    /// Types of the tensor kind
    using TK_TYPES=
      typename TK::types;
    
    // If Compl is a type of the TensKind, return the Conjer
    if constexpr(tupleHasType<Compl,TK_TYPES>)
      return Conjer<T>(forw<T>(smet));
    // Otherwise returns the object itself
    else
      return smet;
  }
  // Simplifies conj(conj)
  CANCEL_DUPLICATED_UNARY_SMET_CALL(conj,Conjer);
  
  // Defines commutativity of Conj with other unary expressions
  UNARY_SMET_GOES_INSIDE(conj,Transposer,transpose);
}

#endif
