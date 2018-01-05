#ifndef _CONJ_HPP
#define _CONJ_HPP

/// \file Conj.hpp
///
/// \brief Header file for the definition of the conjugate of a number

#include <physics/Compl.hpp>
#include <tens/TensKind.hpp>
#include <tex/Transpose.hpp>
#include <tex/UnaryTEx.hpp>

namespace SUNphi
{
  /// Class to take the conjugate of a TEx
  template <typename C,                                  // Type of the expression to conjugate
	    typename TK=typename RemoveReference<C>::Tk, // Tens Kind of the conjugated type
	    typename TK_TYPES=typename TK::types>        // Types of the tensor kind
  class Conjer :
    public UnaryTEx<Conjer<C>>,                    // Inherit from UnaryTEx
    public ConstrainIsTEx<C>,                      // Constrain B to be a TEx
    public ConstrainIsTensKind<TK>,                // Constrain type TK to be a TensKind
    public ConstrainTupleHasType<Compl,TK_TYPES>   // Constrain TG to be in the Types of the TensKind
  {
    
    /// Position of the Compl component
    constexpr static int posOfCompl=posOfType<Compl,TK_TYPES>;
    
  public:
    
  /// Reference to the object to conjugate
    Reference<C> ref;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// TensorKind of the bound expression
    using Tk=TK;
    
    /// Constructor taking a universal reference
    Conjer(C&& ref) : ref(ref)
    {
    }
    
    /// Evaluator
    template <class...Args>
    friend DECLAUTO eval(const Conjer& conjer,
			 const Args&...args)
    {
      STATIC_ASSERT_ARE_N_TYPES(TK::nTypes,args);
      
      // Detect if we have to put "-" in the result
      const bool isIm=get<posOfCompl>(Tuple<Args...>(args...));
      
      const int sign=1-isIm*2;
      
      // Temporary result
      const auto val=eval(conjer.ref,forw<const Args>(args)...);
      
      return sign*val;
    }
    
    
  };
  
  // Check that a test Conjer is a UnaryTEx
  STATIC_ASSERT_IS_UNARY_TEX(Conjer<Tens<TensKind<Compl>,double>>);
  
  // Build Conjer from conj
  SIMPLE_UNARY_TEX_BUILDER(conj,Conjer);
  
  // Simplifies conj(conj)
  CANCEL_DUPLICATED_UNARY_TEX_CALL(conj,Conjer);
  
  // Defines commutativity of Conj with other unary expressions
  UNARY_TEX_GOES_INSIDE(conj,Transposer,transpose);
}

#endif
