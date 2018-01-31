#ifndef _ASSIGN_HPP
#define _ASSIGN_HPP

/// \file Assign.hpp
///
/// \brief Defines a class which assigns rhs SmET to lhs one

#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>
#include <smet/Bind.hpp>
#include <smet/Reference.hpp>

namespace SUNphi
{
  /// Default assigner
  template <typename T1, 	    // Type of the first SmET to get
	    typename T2, 	    // Type of the second SmET to get
	    typename=EnableIf<isSmET<T1> and
			      isSmET<T2> and
			      Unqualified<T1>::isAssignable>,
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void assign(T1&& smet1,        ///< First SmET to act upon
	      T2&& smet2,        ///< Second SmET to act upon
	      SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    // Tens Kind of the rhs expression
    using TKR=typename RemoveReference<T2>::Tk;
    
    // Outermost TensComp of rhs
    using RhsFirstComp=Unqualified<TupleElementType<0,typename TKR::types>>;
    
    // Maximal value reachable from the component
    const int maxEntry=smet2.template compSize<RhsFirstComp>();
    
#ifdef DEBUG_ASSIGN
    using namespace std;
    cout<<" Assign: "<<RhsFirstComp::name()<<" "<<maxEntry;
    cout.flush();
    cout<<", smet1: "<<&smet1;
    cout.flush();
    cout<<", smet2: "<<&smet2;
    cout.flush();
    cout<<", storage1: "<<getStor(smet1)._v;
    cout.flush();
    cout<<", storage2: "<<getStor(smet2)._v;
    cout.flush();
    cout<<endl;
#endif
    
    // Assigns all entries
    for(int i=0;i<maxEntry;i++)
      bind<RhsFirstComp>(forw<T1>(smet1),i)=
	bind<RhsFirstComp>(forw<T2>(smet2),i);
  }
}

#endif
