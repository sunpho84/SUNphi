#ifndef _SCALAR_WRAP_HPP
#define _SCALAR_WRAP_HPP

/// \file ScalarWrap.hpp
///
/// \brief Defines a class which wraps a pure scalar into a SmET, to
/// be used in expressions
///

#include <smet/NnarySmET.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  // Base type to qualify as ScalarWrap
  DEFINE_BASE_TYPE(ScalarWrapper);
  
  /// Class to hold a scalar promoting it into a SmET
  ///
  /// \todo Check what happens if called with a SmET
  template <typename _Fund,                          // Type to be wrapped
	    typename..._Refs>                        // Extra types, need to be empty
  class ScalarWrapper :
    public BaseScalarWrapper,                        // Inherit from BaseTransposer to detect in expression
    public NnarySmET<ScalarWrapper<_Fund,_Refs...>>  // Inherit from NnarySmET
  {
    
  public:
    
    IDENTITY_REPRESENTATIVE_FUNCTION;
    
    /// Returns the size of a component, which is always 1
    int compSize() const
    {
      return 1;
    }
    
    /// Empty TensorKind
    PROVIDE_TK(TensKind<>);
    
    /// Fundamental type
    PROVIDE_FUND(_Fund);
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(0);
    
    /// Type of the scalar ref, reference or not depending on _Fund
    ///
    /// \todo explain
    using ScRef=
      RefIf<isLvalue<_Fund>,RemRef<Fund>>;
    
    /// Provides a reference or a value, depending on \c _Fund
    ScRef scRef;
    
    // Attributes
    STORING;
    IS_ASSIGNABLE_ATTRIBUTE(/*! The SmET is assignable if ref is */,
			    isLvalue<_Fund> and (not isConst<_Fund>));
    PROVIDE_IS_ALIASING(/*! Check with storage */,
			return alias==scRef;);
    
    /// Returns a component-merged version
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Returns the ScalarWrap itself */,
				  return *this);
    
    PROVIDE_MERGEABLE_COMPS(/* There is no component  */,
			    IntSeq<0>);   // Left and right borders
    
    /// Assignement operator
    template <typename Oth>           	// Other type
    ScalarWrapper& operator=(Oth&& oth)	///< Other object
    {
      return (*this)=oth;
    }
    
    /// Provides either the const or non-const evaluator
    template <typename...Args> /// Arguments (need to be empty)
    auto& eval(Args&&...args)
      const
    {
      std::cout<<" is: "<<isLvalue<_Fund><<", "<<isLvalue<decltype(scRef)><<" "<<scRef<<std::endl;
      STATIC_ASSERT_ARE_N_TYPES(0,args);
      
      return scRef;
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(eval);
    
    /// Constructor taking universal reference
    template <typename T,
	      typename=EnableIf<isSame<Unqualified<T>,Unqualified<_Fund>>>>
    explicit ScalarWrapper(T&& val) : scRef(forw<T>(val))
    {
    }
  };
  
  // Check that a test ScalarWrapper is a NnarySmET
  STATIC_ASSERT_IS_NNARY_SMET(ScalarWrapper<double>);
  
  // Build ScalarWrapper from scalarWrap
  SIMPLE_NNARY_SMET_BUILDER(scalarWrap,ScalarWrapper);
  
  // Simplifies scalarWrap(smet) -> smet
  ABSORB_DUPLICATED_NNARY_SMET_CALL(scalarWrap,SmET);
}

#endif
