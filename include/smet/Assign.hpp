#ifndef _ASSIGN_HPP
#define _ASSIGN_HPP

/// \file Assign.hpp
///
/// \brief Defines a class which assigns rhs SmET to lhs one
///
/// These are the steps:
///
/// - The call to \c A=B, with \c A being a \c SmET, is intercepted. If \c A is
///   not assignable an exception is issued. This step is performed in each individual \c SmET,
///   because \c operator= must be a static member function. The function \c assign(A,B) is called.
/// - All call to \c assign where \c B is not a \c SmET are intercepted.
///   If \c B fundamental type is of a type that can be cast to the fundamental
///   type of A, then we return \c A=scalar(B). Otherwise an exception is issued.
/// - If B does not contain all components of \c A, an exception is issued.
/// - Preliminary manipulation is performed, allowing to specific pattern recognition,
///   obtained by overloading \c assign and using appropriate \c SFINAE mechanism.
/// - The default \c assign function is called, which returns the rhs as expected
///   allowing to perform chain assignement on the expected return.
/// - The \c Assigner is created inside assign.
/// - If the \c Assigner has mergeable components, they are merged.
/// - If the innermost component is vectorizable, it is vectorized.
/// - The execution of the assigner is dispatched to the thread pool.
///
#include <tens/TensKind.hpp>
#include <tens/TensClass.hpp>
#include <smet/Bind.hpp>
#include <smet/ScalarWrap.hpp>
#include <smet/Reference.hpp>

namespace SUNphi
{
  /// Default assigner taking only SmET as left argument
  ///
  /// \c Rhs can be a SmEt or not, in which case it is wrapped into a Scalar
  template <typename Lhs, 	    // Type of the l.h.s SmET
	    typename Rhs, 	    // Type of the r.h.s
	    SFINAE_ON_TEMPLATE_ARG(isSmET<Lhs> and
				   Unqualified<Lhs>::isAssignable),
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void assign(Lhs&& lhs,             ///< First SmET to act upon
	      Rhs&& rhs,             ///< Right hand side
	      SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    //take note whether the second member is a SmET, then loop on it
    const bool rhsIsSmET=
      isSmET<Rhs>;
    
    if constexpr(not rhsIsSmET)
      return assign(forw<Lhs>(lhs),scalarWrap(forw<Rhs>(rhs)));
//     else
//       {
// 	// Check that the rhs can be converted into lhs
// 	static_assert(canBeConverted<
// 		      FundTypeOf<Rhs>,
// 		      FundTypeOf<Lhs>>,
// 		      "Need to be able to convert the rhs into lhs fundamental types");
	
// 	if constexpr(TkOf<Lhs>::template contains<TkOf<Rhs>>)
// 	  {
// 	    /// Outermost TensComp of rhs
// 	    using RhsFirstComp=
// 	      Unqualified<TupleElementType<0,typename TkOf<Rhs>::types>>;
	
// 	/// Maximal value reachable from the component
// 	const int maxEntry=
// 	  rhs.template compSize<RhsFirstComp>();
	
// #ifdef DEBUG_ASSIGN
// 	using namespace std;
// 	cout<<" Assign: "<<RhsFirstComp::name()<<" "<<maxEntry;
// 	cout.flush();
// 	cout<<", lhs: "<<&lhs;
// 	cout.flush();
// 	cout<<", rhs: "<<&rhs;
// 	cout.flush();
// 	cout<<", storage1: "<<getStor(lhs)._v;
// 	cout.flush();
// 	cout<<", storage2: "<<getStor(rhs)._v;
// 	cout.flush();
// 	cout<<endl;
// #endif
	
// 	// Assigns all entries
// 	for(int i=0;i<maxEntry;i++)
// 	  bind<RhsFirstComp>(forw<Lhs>(lhs),i)=
// 	    bind<RhsFirstComp>(forw<Rhs>(rhs),i);
//       }
//     else
//       {
// 	/// Tens Kind of the lhs expression
// 	using TKL=
// 	  typename RemoveReference<Lhs>::Tk;
	
// 	/// Outermost TensComp of lhs
// 	using LhsFirstComp=
// 	  Unqualified<TupleElementType<0,typename TKL::types>>;
	
// 	/// Maximal value reachable from the component
// 	const int maxEntry=
// 	  lhs.template compSize<LhsFirstComp>();
	
// 	/// Assigns all entries
// 	for(int i=0;i<maxEntry;i++)
// 	  bind<LhsFirstComp>(forw<Lhs>(lhs),i)=
// 	    rhs;
//       }
//  }
  }
}

#endif
