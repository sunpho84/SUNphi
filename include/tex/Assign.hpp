#ifndef _ASSIGN_HPP
#define _ASSIGN_HPP

/// \file Assign.hpp
///
/// \brief Defines a class which assigns rhs TEx to lhs one

#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>
#include <tex/Bind.hpp>
#include <tex/Reference.hpp>

namespace SUNphi
{
  /// Default assigner
  template <typename T1, 	    // Type of the first TEx to get
	    typename T2, 	    // Type of the second TEx to get
	    typename=EnableIf<IsTEx<T1> and IsTEx<T2> and Unqualified<T1>::isAssignable>,
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void assign(T1&& tex1,        ///< First TEx to act upon
	      T2&& tex2,        ///< Second TEx to act upon
	      SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    using TKR=typename RemoveReference<T2>::Tk; // Tens Kind of the rhs expression
    
    using TkHeadR=Unqualified<TupleElementType<0,typename TKR::types>>;
    const int nComps=TkHeadR::size;
    
    using namespace std;
    cout<<" Assign: "<<TkHeadR::name()<<" "<<nComps;
    cout.flush();
    cout<<", tex1: "<<&tex1;
    cout.flush();
    cout<<", tex2: "<<&tex2;
    cout.flush();
    cout<<", storage1: "<<getStor(tex1)._v;
    cout.flush();
    cout<<", storage2: "<<getStor(tex2)._v;
    cout.flush();
    cout<<endl;
    
    for(int i=0;i<nComps;i++)
	bind<TkHeadR>(forw<T1>(tex1),i)=
	  bind<TkHeadR>(forw<T2>(tex2),i);
  }
}

#endif
