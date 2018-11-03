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
    
    /// Fundamental type
    PROVIDE_FUND(decltype(typename RemRef<Ref<FACT1>>::Fund{}*
			  typename RemRef<Ref<FACT2>>::Fund{}+
			  typename RemRef<Ref<ADDEND>>::Fund{}));
    
    PROVIDE_POS_OF_RES_TCS_IN_REFS;
    
    PROVIDE_MERGING_DELIMS_FOR_REFS;
    
    // PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_TWO_REFS;
    
    // // Returns a component-merged version
    // PROVIDE_GET_MERGED_COMPS_VIEW(/*! Merge appropriately the two references and returns their sum */,
    // 				  return
    // 				  ref1.template getMergedCompsView<MergedDelims1<Is>>()+
    // 				  ref2.template getMergedCompsView<MergedDelims2<Is>>(););
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(MulAdder);
  };
  
  // // Check that a test MulAdder is a BinarySmET
  // namespace CheckMulAdderIsBinarySmet
  // {
  //   /// Tensor comp for test
  //   using MyTc=
  //     TensComp<double,1>;
    
  //   /// Tensor kind to be tested
  //   using MyTk=
  //     TensKind<MyTc>;
    
  //   /// Tensor to be tested
  //   using MyT=
  //     Tens<MyTk,double>;
    
  //   STATIC_ASSERT_IS_BINARY_SMET(MulAdder<MyT,MyT>);
  // }
  
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
