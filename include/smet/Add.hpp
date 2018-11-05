#ifndef _ADD_HPP
#define _ADD_HPP

/// \file Add.hpp
///
/// \brief Defines a class which take the sum of two SmETs

#include <smet/Reference.hpp>
#include <smet/BinarySmET.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>

namespace SUNphi
{
  // Base type to qualify as Adder
  DEFINE_BASE_TYPE(Adder);
  
  /// Class to add two \c SmET
  template <typename _Ref1,                           // First addendum type
	    typename _Ref2,                           // Second addendum type
	    typename TK1=typename RemRef<_Ref1>::Tk,  // \c TensKind of the first addendum type
	    typename TK2=typename RemRef<_Ref2>::Tk>  // \c TensKind of the second addendum type
  class Adder :
    public BaseAdder,                         // Inherit from \c BaseAdder to detect in expression
    public BinarySmET<Adder<_Ref1,_Ref2>>,    // Inherit from \c BinarySmET
    public ConstrainAreTensKinds<TK1,TK2>     // Constrain type TK1 and TK2 to be a TensKind
  {
    STATIC_ASSERT_IS_SMET(RemRef<_Ref1>);
    STATIC_ASSERT_IS_SMET(RemRef<_Ref2>);
    
  public:
    
    /// Returns the size of a component
    template <typename TC>
    int compSize() const
    {
      // Returns the size if it's in the first Tk or in the second
      if constexpr(tupleHasType<TC,typename TK1::types>)
	 return ref1.template compSize<TC>();
      else
	 return ref2.template compSize<TC>();
    }
    
    PROVIDE_BINARY_SMET_REFS;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_PAIR_OF_REFS;
    
    /// TensorKind of the result
    ///
    /// \todo Improve, the return type could be decided studying what
    /// suits best the computational
    PROVIDE_TK(BlendTensKinds<TK1,TK2>);
    
    /// Fundamental type
    PROVIDE_FUND(decltype(typename RemRef<Ref1>::Fund{}+
			  typename RemRef<Ref2>::Fund{}));
    
    PROVIDE_POS_OF_TCS_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(1);
    PROVIDE_POS_OF_TCS_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(2);
    
    PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_TWO_REFS;
    
    // Returns a component-merged version
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Merge appropriately the two references and returns their sum */,
    				  return
				   ref1.template getMergedCompsView<MergedDelims1<Is>>()+
				   ref2.template getMergedCompsView<MergedDelims2<Is>>(););
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER)		\
    /*! QUALIFIER evaluator for Adder                                */	\
    /*!                                                              */	\
    /*! Internal Evaluator, splitting the id following the position  */ \
    /*! of component in the two ref                                  */	\
    template <typename...Args, /* Type of the arguments              */	\
	      int...Comps1,    /* Position of the first set of args  */ \
	      int...Comps2>    /* Position of the second set of args */ \
    DECLAUTO adder_internal_eval(IntSeq<Comps1...>,            /*!< List of position of components for ref1 */ \
				 IntSeq<Comps2...>,              /*!< List of position of components for ref2 */ \
				 const Tuple<Args...>& targs)  /*!< Components to get                       */ \
      QUALIFIER								\
    {									\
      return								\
	ref1.eval(get<Comps1>(targs)...)+				\
	ref2.eval(get<Comps2>(targs)...);				\
    }									\
									\
    /*! Evaluator, external interface                                */	\
    /*!                                                              */	\
    /*! Pass to the internal implementation the integer sequence     */	\
    /*! needed to split the components                               */	\
    template <typename...Args>           /* Type of the arguments    */	\
    DECLAUTO eval(const Args&...args)    /*!< Components to get      */	\
      QUALIFIER								\
    {									\
      STATIC_ASSERT_ARE_N_TYPES(Tk::nTypes,args);			\
      									\
      return adder_internal_eval(PosOfRef1PresTcsInResTk{},		\
				 PosOfRef2PresTcsInResTk{},		\
				 std::forward_as_tuple(args...));	\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR
    
    PROVIDE_BINARY_SMET_SIMPLE_CREATOR(Adder);
  };
  
  // Check that a test Adder is a BinarySmET
  namespace CheckAdderIsBinarySmet
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
    
    STATIC_ASSERT_IS_BINARY_SMET(Adder<MyT,MyT>);
  }
  
  // Build Adder from add
  SIMPLE_BINARY_SMET_BUILDER(add,Adder);
  
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
