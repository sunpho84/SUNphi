#ifndef _CONJ_HPP
#define _CONJ_HPP

/// \file Conj.hpp
///
/// \brief Header file for the definition of the conjugate of a number

#include <physics/Compl.hpp>
#include <ios/Logger.hpp>
#include <tens/TensKind.hpp>
#include <smet/NnarySmET.hpp>
#include <smet/Transpose.hpp>

namespace SUNphi
{
  // Base type to qualify as Conjer
  DEFINE_BASE_TYPE(Conjer);
  
  /// Class to take the conjugate of a \c SmET
  template <typename..._Refs>                         // Type of the expression to conjugate
  class Conjer :
    public BaseConjer,                                // Inherit from \c BaseConjer to detect in expression
    public NnarySmET<Conjer<_Refs...>>,               // Inherit from \c NnarySmET
    public ConstrainAreSmETs<_Refs...>                // Constrain all \c _Refs to be \c SmET
  {
    
  public:
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(1);
    
    /// Position of the Compl component
    constexpr static int posOfCompl=
      posOfType<Compl,typename TkOf<Ref<0>>::types>;
    
    PROVIDE_SIMPLE_NNARY_COMP_SIZE;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    /// TensorKind of the bound expression
    SAME_TK_AS_REF(0);
    
    SAME_FUND_AS_REF(0);
    
    PROVIDE_POS_OF_RES_TCS_IN_REFS;
    
    PROVIDE_EXTRA_MERGE_DELIMS(IntSeq<posOfCompl,posOfCompl+1>);
    
    PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_REFS_AND_EXTRA;
    
    PROVIDE_NNARY_GET_MERGED_COMPS_VIEW_ACCORDING_TO_REPRESENTATIVE_FUNCTION;
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(Conjer);
    
    /// Evaluator for \c Conjer
    template <typename...Args>    // Type of the arguments
    auto eval(Args&&...args)    //!< Components to get
      const
    {
      STATIC_ASSERT_ARE_N_TYPES(TkOf<Ref<0>>::nTypes,args);
      
      /// Detect if we have to put "-" in the result
      const bool isIm=
	get<posOfCompl>(Tuple<Args...>(forw<Args>(args)...));
      
      /// Temporary result
      auto val=
	get<0>(refs).eval(forw<Args>(args)...);
      
      if(isIm)
      	return -val;
      else
      	return +val;
    }
  };
  
  // Check that a test Conjer is a NnarySmET
  STATIC_ASSERT_IS_NNARY_SMET(Conjer<Tens<TensKind<Compl>,double>>);
  
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
  CANCEL_DUPLICATED_NNARY_SMET_CALL(conj,Conjer);
  
  // Defines commutativity of Conj with other nnary expressions
  NNARY_SMET_GOES_INSIDE(conj,Transposer,transpose);
}

#endif
