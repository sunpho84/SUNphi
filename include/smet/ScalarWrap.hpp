#ifndef _SCALAR_WRAP_HPP
#define _SCALAR_WRAP_HPP

/// \file ScalarWrap.hpp
///
/// \brief Defines a class which wraps a pure scalar into a SmET, to
/// be used in expressions
///

#include <smet/UnarySmET.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  // Base type to qualify as ScalarWrap
  DEFINE_BASE_TYPE(ScalarWrapper);
  
  /// Class to hold a scalar promoting it into a SmET
  ///
  /// \todo Check what happens if called with a SmET
  template <typename _Ref>                        // Type to be wrapped
  class ScalarWrapper :
    public BaseScalarWrapper,                     // Inherit from BaseTransposer to detect in expression
    public UnarySmET<ScalarWrapper<_Ref>>         // Inherit from UnarySmET
  {
    
  public:
    
    /// Returns the size of a component, which is always 1
    ///
    /// The result is the size of the twinned component
    int compSize() const
    {
      return 1;
    }
    
    /// Type of the reference
    using Ref=
      _Ref;
    
    /// Provides a reference or a value, depending on _Ref
    RefIf<isLvalue<_Ref>,RemoveReference<_Ref>> ref;
    
    // Attributes
    STORING;
    ASSIGNABLE_ACCORDING_TO_REF;
    PROVIDE_IS_ALIASING(/*! Check with storage */,
			return alias==ref;);
    
    /// TensorKind of the bound expression
    using Tk=
      TensKind<>;
    
    PROVIDE_MERGEABLE_COMPS(/* There is no component  */,
			    IntSeq<0>);   // Left and right borders
    
    PROVIDE_UNARY_SMET_SIMPLE_GET_MERGED_COMPS_VIEW(ScalarWrapper);
    
    /// Assignement operator
    template <typename Oth>           	// Other type
    ScalarWrapper& operator=(Oth&& oth)	///< Other object
    {
      return (*this)=oth;
    }
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER)		\
    /*! QUALIFIER evaluator for ScalarWrapper                        */	\
    template <typename...Args> /*! Arguments (need to be empty)      */ \
    DECLAUTO eval(Args&&...args)					\
      QUALIFIER								\
    {									\
      STATIC_ASSERT_ARE_N_TYPES(0,args);				\
									\
      return ref;							\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(/* Non const*/);
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(const);
    
#undef PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR
    
    PROVIDE_UNARY_SMET_SIMPLE_CREATOR(ScalarWrapper);
  };
  
  // Check that a test ScalarWrapper is a UnarySmET
  STATIC_ASSERT_IS_UNARY_SMET(ScalarWrapper<double>);
  
  // Build ScalarWrapper from scalarWrap
  SIMPLE_UNARY_SMET_BUILDER(scalarWrap,ScalarWrapper);
  
  // Simplifies scalarWrap(scalarWrap)
  CANCEL_DUPLICATED_UNARY_SMET_CALL(scalarWrap,ScalarWrapper);
}

#endif
