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

#include <smet/BinarySmET.hpp>
#include <smet/Bind.hpp>
#include <smet/Reference.hpp>
#include <smet/ScalarWrap.hpp>
#include <tens/TensClass.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  // Base type to qualify as an \c Assigner
  DEFINE_BASE_TYPE(Assigner);
  
  /// Class to assign a \c SmET to another one
  template <typename _Ref1,                           // Type of l.h.s
	    typename _Ref2,                           // Type of r.h.s
	    typename TK1=typename RemRef<_Ref1>::Tk,  // \c TensKind of the l.h.s
	    typename TK2=typename RemRef<_Ref2>::Tk>  // \c TensKind of the r.h.s
  class Assigner :
    public BaseAssigner,                          // Inherit from \c BaseAssigner to detect in expression
    public BinarySmET<Assigner<_Ref1,_Ref2>>      // Inherit from \c BinarySmET
  {
    STATIC_ASSERT_IS_SMET(RemRef<_Ref1>);
    STATIC_ASSERT_IS_SMET(RemRef<_Ref2>);
    
    // Check that the r.h.s Fund type can be converted into l.h.s
    static_assert(canBeConverted<
		  FundTypeOf<_Ref2>,
		  FundTypeOf<_Ref1>>,
		  "Need to be able to convert the r.h.s into l.h.s fundamental types");
    
  public:
    
    /// Returns the size of a component
    template <typename TC,
	      typename=ConstrainIsTensComp<TC>>
    int compSize() const
    {
      // Returns the size if it's in the l.h.s \c Tk or in the r.h.s one
      if constexpr(tupleHasType<TC,TK1>)
	 return
	   ref1.template compSize<TC>();
      else
	CRASH<<"Cannot refer to a TensComp which is not in the l.h.s";
    }
    
    PROVIDE_BINARY_SMET_REFS;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_PAIR_OF_REFS;
    
    /// \c TensorKind of the result is the same of l.h.s
    PROVIDE_TK(TkOf<_Ref1>);
    
    /// Fundamental type is of the same type of the l.h.s
    PROVIDE_FUND(FundTypeOf<Ref1>);
    
    PROVIDE_POS_OF_TCS_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(1);
    PROVIDE_POS_OF_TCS_IN_RES_TK_AND_MERGED_DELIMS_FOR_REF(2);
    
    PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_TWO_REFS;
    
    // Returns a component-merged version
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Merge appropriately the two references and returns their assignement */,
    				  return
				  ref1.template getMergedCompsView<MergedDelims1<Is>>()=
				  ref2.template getMergedCompsView<MergedDelims2<Is>>(););
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER)		\
    /*! QUALIFIER evaluator for Assigner, evaluating r.h.s           */	\
    template <typename...Args>           /* Type of the arguments    */	\
    DECLAUTO eval(const Args&...args)    /*!< Components to get      */	\
      QUALIFIER								\
    {									\
      STATIC_ASSERT_ARE_N_TYPES(Tk::nTypes,args);			\
      									\
      return ref2.eval(forw<Args>(args)...);				\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR
    
    PROVIDE_BINARY_SMET_SIMPLE_CREATOR(Assigner);
  };
  
  /// Default assigner taking only \c SmET as left argument
  ///
  /// \c Rhs can be a \c SmET or not, in which case it is wrapped into a Scalar
  template <typename Lhs, 	    // Type of the l.h.s \c SmET
	    typename Rhs, 	    // Type of the r.h.s
	    SFINAE_ON_TEMPLATE_ARG(isSmET<Lhs> and
				   Unqualified<Lhs>::isAssignable),
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void assign(Lhs&& lhs,             ///< Left hand side, \c SmET to act upon
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
// 	  typename RemRef<Lhs>::Tk;
	
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
