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
    
    // Tens Kind of the rhs expression
    using TKR=typename RemoveReference<T2>::Tk;
    
    // Outermost TensComp of rhs
    using RhsFirstComp=Unqualified<TupleElementType<0,typename TKR::types>>;
    
    // Maximal value reachable from the component
    const int maxEntry=RhsFirstComp::size;
    static_assert(maxEntry>=0,"Dynamic case not still taken into account, we need to think on it");
    
#ifdef DEBUG_ASSIGN
    using namespace std;
    cout<<" Assign: "<<RhsFirstComp::name()<<" "<<maxEntry;
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
#endif
    
    // Assigns all entries
    for(int i=0;i<maxEntry;i++)
      bind<RhsFirstComp>(forw<T1>(tex1),i)=
	bind<RhsFirstComp>(forw<T2>(tex2),i);
  }
}

#endif
