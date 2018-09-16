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
	    typename T2, 	    // Type of the rhs
	    typename=EnableIf<isSmET<T1> and
			      Unqualified<T1>::isAssignable>,
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void assign(T1&& lhs,             ///< First SmET to act upon
	      T2&& rhs,             ///< Right hand side
	      SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    //take note whether the second member is a SmET, then loop on it
    const bool rhsIsSmET=isSmET<T2>;
    
    
    if constexpr(rhsIsSmET)
      {
	// Tens Kind of the rhs expression
	using TKR=typename RemoveReference<T2>::Tk;
	
	// Outermost TensComp of rhs
	using RhsFirstComp=Unqualified<TupleElementType<0,typename TKR::types>>;
	
	// Maximal value reachable from the component
	const int maxEntry=rhs.template compSize<RhsFirstComp>();
	
#ifdef DEBUG_ASSIGN
	using namespace std;
	cout<<" Assign: "<<RhsFirstComp::name()<<" "<<maxEntry;
	cout.flush();
	cout<<", lhs: "<<&lhs;
	cout.flush();
	cout<<", rhs: "<<&rhs;
	cout.flush();
	cout<<", storage1: "<<getStor(lhs)._v;
	cout.flush();
	cout<<", storage2: "<<getStor(rhs)._v;
	cout.flush();
	cout<<endl;
#endif
	
	// Assigns all entries
	for(int i=0;i<maxEntry;i++)
	  bind<RhsFirstComp>(forw<T1>(lhs),i)=
	    bind<RhsFirstComp>(forw<T2>(rhs),i);
      }
    else
      {
	// Tens Kind of the lhs expression
	using TKL=typename RemoveReference<T1>::Tk;
	
	// Outermost TensComp of lhs
	using LhsFirstComp=Unqualified<TupleElementType<0,typename TKL::types>>;
	
	// Maximal value reachable from the component
	const int maxEntry=lhs.template compSize<LhsFirstComp>();
	
	// Assigns all entries
	for(int i=0;i<maxEntry;i++)
	  bind<LhsFirstComp>(forw<T1>(lhs),i)=
	    rhs;
      }
  }
}

#endif
