#ifndef _ADD_HPP
#define _ADD_HPP

/// \file Add.hpp
///
/// \brief Defines a class which take the sum of two SmETs

#include <smet/Reference.hpp>
#include <smet/BinarySmET.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  // Base type to qualify as Adder
  DEFINE_BASE_TYPE(Adder);
  
  /// Class to take the added of a SmET
  template <typename _Ref1,                                    // First addendum type
	    typename _Ref2,                                    // Second addendum type
	    typename TK1=typename RemoveReference<_Ref1>::Tk,  // Tens Kind of the first addendum type
	    typename TK2=typename RemoveReference<_Ref2>::Tk>  // Tens Kind of the second addendum type
  class Adder :
    public BaseAdder,                         // Inherit from BaseAdder to detect in expression
    public BinarySmET<Adder<_Ref1,_Ref2>>,    // Inherit from BinarySmET
    public ConstrainAreTensKinds<TK1,TK2>     // Constrain type TK1 and TK2 to be a TensKind
  {
    STATIC_ASSERT_IS_SMET(RemoveReference<_Ref1>);
    STATIC_ASSERT_IS_SMET(RemoveReference<_Ref2>);
    
  public:
    
    /// Returns the size of a component
    template <typename TC>
    int compSize() const
    {
      if constexpr(tupleHasType<TC,TK1::types>)
	 return ref1.template compSize<TC>();
      else
	 return ref2.template compSize<TC>();
    }
    
    PROVIDE_BINARY_SMET_REFS;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    /// TensorKind of the result
    using Tk=BlendTensKinds<TK1,TK2>;
    
    /// \todo fix it
    //NOT_MERGEABLE;
    
    PROVIDE_BINARY_SMET_SIMPLE_CREATOR(Adder);
  };
  
  // Check that a test Adder is a UnarySmET
  //STATIC_ASSERT_IS_UNARY_SMET(Adder<Tens<TensKind<TensComp<double,1>>,double>>);
  
  // Build Adder from add
  SIMPLE_BINARY_SMET_BUILDER(add,Adder);
  
  /// Implement smet1+smet2
  template <typename T1,              // Type of the first expression
	    typename T2,              // Type of the second expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T1> and isSmET<T2>)>
  DECLAUTO operator+(T1&& smet1,T2&& smet2)       ///< Expression
  {
    return add(forw<T1>(smet1),forw<T2>(smet2));
  }
}

#endif
