#ifndef _ASSIGN_HPP
#define _ASSIGN_HPP

/// \file Assign.hpp
///
/// \brief Defines a class which assigns rhs TEx to lhs one

#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>
#include <tex/BinaryTEx.hpp>
#include <tex/Bind.hpp>
#include <tex/Reference.hpp>

namespace SUNphi
{
  // Base type to qualify as Assigner
  DEFINE_BASE_TYPE(Assigner);
  
  /// Class to wrap a TEx
  /// \todo add a check on the matching of Lhs Tk with Rhs
  template <typename _Ref1,                                   // Type of lhs expression to assign
	    typename _Ref2,                                   // Type of rhs expression to assign
	    typename TKL=typename RemoveReference<_Ref1>::Tk, // Tens Kind of the lhs expression
	    typename TKR=typename RemoveReference<_Ref1>::Tk> // Tens Kind of the rhs expression
  class Assigner :
    public BaseAssigner,                              // Inherit from BaseWrapper to detect in expression
    public BinaryTEx<Assigner<_Ref1,_Ref2>>           // Inherit from BinaryTEx
  {
    static_assert(Unqualified<_Ref1>::isAssignable,"Needs a lhs that can be assigned");
    
    STATIC_ASSERT_IS_TEX(Unqualified<_Ref1>);         // Constrain _Ref1 to be a TEx
    STATIC_ASSERT_IS_TEX(_Ref2);                      // Constrain _Ref1 to be a TEx
    STATIC_ASSERT_IS_BASE_OF(BaseTensKind,TKL);       // Constrain type TKL to be a TensKind
    STATIC_ASSERT_IS_BASE_OF(BaseTensKind,TKR);       // Constrain type TKL to be a TensKind
    
  public:
    
    /// TensorKind of the bound expression, same of lhs
    using Tk=TKL;
    
    /// Type of lhs
    using Ref1=_Ref1;
    
    /// Type of rhs
    using Ref2=_Ref2;
    
    /// Reference to lhs
    Reference<Ref1> ref1;
    
    /// Reference to rhs
    const Ref2& ref2;
    
    // Attributes
    NOT_STORING;
    NOT_ASSIGNABLE;
    FORWARD_IS_ALIASING_TO_REF_1_2;
    
    /// Default constructor for Assigner
    explicit Assigner(Ref1&& ref1,      ///< Lhs of the assignement
		      const Ref2& ref2) ///< Rhs of the assignement
      :
      ref1(ref1),
      ref2(ref2)
    {
    }
    
    /// Close the assignement
    ///
    /// Internal implementation, called when all the components are
    /// bound and the only missing things is to eval the
    /// subexpressions.
    template <bool Straight>
    void _close(SFINAE_ON_TEMPLATE_ARG(Straight))
    {
      using namespace std;
      //cout<<"Cic "<<__PRETTY_FUNCTION__<<endl;
      ref1.eval()=ref2.eval();
    }
    
    /// Close the assignement
    ///
    /// Internal implementation, called when not all components are
    /// bound. Close the expressione iteratively, looping on the
    /// outermost component. A new Assigner is created at every turn.
    template <bool Straight>
    void _close(SFINAE_ON_TEMPLATE_ARG(not Straight))
    {
      using TkHeadR=Unqualified<decltype(get<0>(typename TKR::types{}))>;
      const int nComps=TkHeadR::size;
      
      for(int i=0;i<nComps;i++)
	{
	  auto l=bind<TkHeadR>(ref1,i);
	  auto r=bind<TkHeadR>(ref2,i);
	  auto a=assign(l,r);
	  a.close();
	}
    }
    
    /// Close the assignement
    ///
    /// External interface, called to disambiguate the bound and
    /// unbound case
    void close()
    {
      const bool straightAssign=(Unqualified<TKL>::nTypes==0) and (TKR::nTypes==0);
      _close<straightAssign>();
    }
  };
  
  // Check that a test Assigner is a BinaryTEx
  STATIC_ASSERT_IS_BINARY_TEX(Assigner<
  			      Tens<TensKind<TensComp<double,3>>,double>,
  			      Tens<TensKind<TensComp<double,3>>,double>>);
  /// Default constructor
  template <typename T1, 	    // Type of the first TEx to get
	    typename T2, 	    // Type of the second TEx to get
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  Assigner<T1,T2> assign(T1&& tex1,        ///< First TEx to act upon
			 const T2& tex2,   ///< Second TEx to act upon
			 SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    return Assigner<T1,T2>(forw<T1>(tex1),tex2);
  }
}

#endif
