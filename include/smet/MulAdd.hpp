#ifndef _MUL_ADD_HPP
#define _MUL_ADD_HPP

/// \file MulAdd.hpp
///
/// \brief Defines a class which take the sum of a \c SmET with the product of two others
///
/// For the moment this is just used to provide a testcase to write the generic Nnary

#include <smet/Reference.hpp>
#include <smet/NnarySmET.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>

namespace SUNphi
{
  // Base type to qualify as MulAdder
  DEFINE_BASE_TYPE(MulAdder);
  
  /// Class to add a \c SmET with the prodcut of two others: \c a+b*c
  template <typename..._Refs>                    // Reference types
  class MulAdder :
    public BaseMulAdder,                         // Inherit from \c BaseMulAdder to detect in expression
    public NnarySmET<MulAdder<_Refs...>>         // Inherit from \c NnarySmET
  {
    static_assert((isSmET<RemRef<_Refs>> && ...),"All types must be SmET");
    
  public:
    
    /// Position of the elements
    enum Pos_t{FACT1,
	       FACT2,
	       ADDEND};
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(3);
    
    PROVIDE_SIMPLE_NNARY_COMP_SIZE;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    /// TensorKind of the result
    ///
    /// \todo fixit
    PROVIDE_TK(typename RemRef<Ref<ADDEND>>::Tk);
    
    PROVIDE_POS_OF_RES_TCS_IN_REFS;
    
    /// Fundamental type
    PROVIDE_FUND(decltype(typename RemRef<Ref<FACT1>>::Fund{}*
			  typename RemRef<Ref<FACT2>>::Fund{}+
			  typename RemRef<Ref<ADDEND>>::Fund{}));
    
    NO_EXTRA_MERGE_DELIMS;
    
    PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_REFS_AND_EXTRA;
    
    PROVIDE_NNARY_GET_MERGED_COMPS_VIEW(/*! Merge appropriately the three references and returns their combination */,
					return
					  MERGED_COMPS_VIEW_OF_REF(FACT1)*
					  MERGED_COMPS_VIEW_OF_REF(FACT2)+
					  MERGED_COMPS_VIEW_OF_REF(ADDEND));
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(MulAdder);
   
    template <typename Fact1,
	      typename Fact2,
	      typename Addend>
    static DECLAUTO representativeFunction(Fact1&& fact1,
					   Fact2&& fact2,
					   Addend&& addend)
    {
      return fact1*fact2+addend;
    }
    
    EVAL_THROUGH_REPRESENTATIVE_FUNCTION_PASSING_COMPS_BY_NAME;
  };
  
  // Check that a test MulAdder is a NnarySmET
  namespace CheckMulAdderIsBinarySmet
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
    
    STATIC_ASSERT_IS_NNARY_SMET(MulAdder<MyT,MyT,MyT>);
  }
  
  // Build MulAdder from mulAdd
  SIMPLE_NNARY_SMET_BUILDER(mulAdd,MulAdder);
  
  // /// Implement smet1+smet2
  // template <typename T1,              // Type of the first expression
  // 	    typename T2,              // Type of the second expression
  // 	    SFINAE_ON_TEMPLATE_ARG(isSmET<T1> and isSmET<T2>)>
  // DECLAUTO operator+(T1&& smet1,      ///< First addend
  // 		     T2&& smet2)      ///< Second addend
  // {
  //   return add(forw<T1>(smet1),forw<T2>(smet2));
  // }
}

#endif
