#ifndef _ADD_HPP
#define _ADD_HPP

/// \file Add.hpp
///
/// \brief Defines a class which take the sum of two SmETs

#include <smet/Reference.hpp>
#include <smet/NnarySmET.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>

namespace SUNphi
{
  // Base type to qualify as Adder
  DEFINE_BASE_TYPE(Adder);
  
  /// Class to add two \c SmET
  template <typename..._Refs>                // Addendum types
  class Adder :
    public BaseAdder,                        // Inherit from \c BaseAdder to detect in expression
    public NnarySmET<Adder<_Refs...>>,       // Inherit from \c NnarySmET
    public ConstrainAreSmETs<_Refs...>       // Constrain all \c Refs to be \c SmET
  {
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(2);
    
  public:
    
    /// Position of the references
    enum Pos_t{ADDEND1,
	       ADDEND2};
    
    /// Representative function of the sum operation
    template <typename Addend1,  // Type of the first addend
	      typename Addend2>  // Type of the second addend
    static DECLAUTO representativeFunction(Addend1&& addend1,   ///< First addend
					   Addend2&& addend2)   ///< Second addend
    {
      return addend1+addend2;
    }
    
    PROVIDE_SIMPLE_NNARY_COMP_SIZE;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    /// \c TensKind of the result
    ///
    /// \todo Improve, the return type could be decided studying what
    /// suits best the computational
    PROVIDE_TK(BlendTensKinds<TkOf<Ref<ADDEND1>>,TkOf<Ref<ADDEND2>>>);
    
    PROVIDE_FUND_ACCORDING_TO_REPRESENTATIVE_FUNCTION;
    
    PROVIDE_POS_OF_RES_TCS_IN_REFS;
    
    NO_EXTRA_MERGE_DELIMS;
    
    PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_REFS_AND_EXTRA;
    
    PROVIDE_NNARY_GET_MERGED_COMPS_VIEW_ACCORDING_TO_REPRESENTATIVE_FUNCTION;
    
    EVAL_THROUGH_REPRESENTATIVE_FUNCTION_PASSING_COMPS_BY_NAME;
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(Adder);
  };
  
  // Check that a test Adder is a \c NnarySmET
  namespace CheckAdderIsNnarySmet
  {
    /// Tensor comp for test
    using MyTc=
      TensComp<double,1>;
    
    /// Tensor kind to be tested
    using MyTk=
      TensKind<MyTc>;
    
    /// Tensor to be tested
    using MyT=
      Tens<MyTk,double>;
    
    STATIC_ASSERT_IS_NNARY_SMET(Adder<MyT,MyT>);
  }
  
  // Build Adder from add
  SIMPLE_NNARY_SMET_BUILDER(add,Adder);
  
  /// Implement smet1+smet2
  template <typename T1,              // Type of the first expression
	    typename T2,              // Type of the second expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T1> and isSmET<T2>)>
  DECLAUTO operator+(T1&& smet1,      ///< First addend
		     T2&& smet2)      ///< Second addend
  {
    return add(forw<T1>(smet1),forw<T2>(smet2));
  }
}

#endif
