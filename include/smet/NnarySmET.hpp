#ifndef _NNARYSMET_HPP
#define _NNARYSMET_HPP

/// \file NnarySmET.hpp
///
/// \brief Header file defining basic properties of Nnary SmET

#include <ints/IntSeqCat.hpp>
#include <smet/BaseSmET.hpp>
#include <tens/TensKind.hpp>
#include <tuple/TupleOrder.hpp>
#include <utility/Position.hpp>

namespace SUNphi
{
  /// Defines the NnarySmET type traits
  DEFINE_BASE_TYPE(NnarySmET,: public BaseSmET);
  
  // Defines the check for a member type "refs"
  DEFINE_HAS_MEMBER(refs);
  
  /// Defines the check for a Nnary SmET
#define STATIC_ASSERT_IS_NNARY_SMET(...)		\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(refs,__VA_ARGS__)
  
  /// Provide an identity representative function
#define IDENTITY_REPRESENTATIVE_FUNCTION				\
  /*! Returns the argument */						\
  template <typename T>	                        /* Argument type */	\
  static DECLAUTO representativeFunction(T&& t) /*!< Argument    */	\
  {									\
    return forw<T>(t);							\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Nnary SmET
  template <typename T>  /// Inheriting type
  struct NnarySmET :
    public SmET<T>,
    public BaseNnarySmET
  {
    IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can never be lhs */,false);
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Evaluate the I-th reference, getting from the \c Tuple targs the element \c Pos
    ///
    /// Internal implementation
    template <int I,       // Reference to evaluate
	      typename Tp, // Type of the \c Tuple containing the components to call, i
	      int...Pos>   // Position of the args
    DECLAUTO _refEvalByCompsName(IntSeq<I>,
				 IntSeq<Pos...>,    ///< List of position of components for ref
				 Tp&& targs) const  ///< Components to get
    {
      return get<I>((~*this).refs).eval(get<Pos>(targs)...);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(_refEvalByCompsName);
    
    /// Evaluate the I-th reference, getting from the \c Tuple targs the element \c Pos
    ///
    /// External implementation, using \c PosOfResTcsPresInRefsTk to dispatch components
    template <int I,       // Reference to evaluate
	      typename Tp> // Type of the \c Tuple containing the components to call
    DECLAUTO refEvalByCompsName(Tp&& targs) const  ///< Components to get
    {
      /// Position of \c TensComp of the reference Tk present in thr result
      using Pos=
	TupleElementType<I,typename T::PosOfResTcsPresInRefsTk>;
      
      return _refEvalByCompsName(IntSeq<I>{},Pos{},forw<Tp>(targs));
    }
    
    /// Evaluate the I-th reference, getting from the \c Tuple targs the element \c Pos
    ///
    /// Non const version
    template <int I,       // Reference to evaluate
	      typename Tp> // Type of the \c Tuple containing the components to call, i
    DECLAUTO refEvalByCompsName(Tp&& targs)
    {
      return CALL_CLASS_CONST_METHOD_REMOVING_CONST(refEvalByCompsName<I>(forw<Tp>(targs)));
    }
    
    /// Evaluate the result by calling a representative function
    ///
    /// The result is obtained after splitting the components,
    /// identifying them trough the name of each \c TensComp
    template <int...I,
	      typename...Args>    // Type of the arguments
    DECLAUTO evalThroughRepresentativeFunctionPassingCompsByName(IntSeq<I...>,               ///< Dummy \c IntSeq to infer I
								 const Args&...args) const   ///< Components to get
    {
      STATIC_ASSERT_ARE_N_TYPES(T::Tk::nTypes,args);
      
      return (~*this).representativeFunction(this->refEvalByCompsName<I>(std::forward_as_tuple(args...))...);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(evalThroughRepresentativeFunctionPassingCompsByName);
  };
  
  /// Provides an evaluator through a representative function
#define EVAL_THROUGH_REPRESENTATIVE_FUNCTION_PASSING_COMPS_BY_NAME	\
  /*! Evaluator, external interface                              */	\
  /*!                                                            */	\
  /*! Evaluate the function by usign a method called             */	\
  /*! \c representativeFunction, to be provided in the clas      */	\
  /*! passing to it the evaluation of each individual reference, */	\
  /*! dispatching the components id by the name they have in the */	\
  /*! reference                                                  */	\
  template <typename...Args>          /* Type of the arguments   */	\
  DECLAUTO eval(Args&&...args)  const /* Components to get       */	\
  {									\
    return this->evalThroughRepresentativeFunctionPassingCompsByName(IntsUpTo<NSmET>{},	\
								     forw<Args>(args)...); \
  }									\
									\
  PROVIDE_ALSO_NON_CONST_METHOD(eval)
  
  /// Provide the references to the objects
  ///
  /// The reference types are contained in a \c Tuple with types
  /// obtained from template pars
#define PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(N)	\
  /*! Type of the binding references */			\
  using Refs=						\
    Tuple<_Refs...>;					\
  							\
  /*! Reference to the N object */			\
  Tuple<Reference<_Refs>...> refs;			\
  							\
  /*! Type of I-th \c SmET      */			\
  template <int I>					\
  using Ref=						\
    TupleElementType<I,Tuple<_Refs...>>;		\
							\
  /*! Number or \c SmET to act upon */			\
  static constexpr					\
  int NSmET=						\
    N;							\
							\
  /* Check that NSmET coincides with the par */		\
  static_assert(N==sizeof...(_Refs))
  
  /// Returns the size of a \c TensComp, with a simple approach
  ///
  /// Search the asked \c TensComp in each ref \c TensKind iteratively
  /// and returns the size in the first found ref
#define PROVIDE_SIMPLE_NNARY_COMP_SIZE					\
  /*! Returns the size in the first found ref */			\
  template <typename TC, /* \c TensComp to search           */		\
	    int I=0>     /* Integer of the ref to search in */		\
  int compSize() const							\
  {									\
    static_assert(I>=0 and I<NSmET,"Cannot search in negative or larger-than-N ref"); \
									\
    /*! \c TensKind of the I-th \c SmET */				\
    using RefTk=							\
      typename Ref<I>::Tk;						\
									\
    /*! Check if the \c TensKind contains the \c TensComp asked */	\
    constexpr bool found=						\
      RefTk::template contains<TC>;					\
									\
    /* Returns the size if it's in the I-th Tk */			\
    if constexpr(found)							\
      return get<I>(refs).template compSize<TC>();		        \
    else								\
      return compSize<TC,I+1>();					\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Defines the \c Fund type using the call to \c representativeFunction
#define PROVIDE_FUND_ACCORDING_TO_REPRESENTATIVE_FUNCTION		\
  /*! Fundamental type according to the \c representativeFunction */	\
  /*!                                                             */	\
  /*! Internal implementation, getting the type through a call to */	\
  /*! the \c representativeFunction passing by value the \c Fund  */	\
  /*! type of each ref, using an \c IntSeq to call them           */	\
  template <int...Is>							\
  static DECLAUTO _fundThroughRepresentativeFunction(IntSeq<Is...>)	\
  {									\
    return representativeFunction(typename RemRef<Ref<Is>>::Fund{}...); \
  }									\
  									\
  /*! Obtains the \c Fund type by calling the \c representativeFunction */ \
  using Fund=								\
    decltype(_fundThroughRepresentativeFunction(IntsUpTo<NSmET>{}))
  
  /// Get the same \c Fund type of a given ref
#define SAME_FUND_AS_REF(ID)			\
  PROVIDE_FUND(typename RemRef<Ref<ID>>::Fund)
  
  /// Provide the position of result Tk \c TensComp in each input
#define PROVIDE_POS_OF_RES_TCS_IN_REFS					\
  /*! Position of all the Result \c TensComp in each \c Refs Tk */	\
  using PosOfResTcsInRefsTk=						\
    PosOfTcsOfTkInListOfTks<Tk,typename RemRef<_Refs>::Tk...>;		\
									\
  /*! Position of all the Result \c TensComp present in each \c Refs Tk */	\
  using PosOfResTcsPresInRefsTk=						\
    PosOfTcsOfTkPresInListOfTks<Tk,typename RemRef<_Refs>::Tk...>
  
  /// Determine the mergeability of the \c ResPos component of the result
  ///
  /// Nested internal implemention
  template <int ResPos=0,                    // Current \c TensComp to be checked
	    typename...MergeDelim,           // \c IntSeq containing the allowed mergeability delimiters for each Ref
	    typename...PosOfResTcsInRefTk,   // Route to give the position of each \c TensComp of the Res in each Ref
	    int...PrevPosInts,               // Position of previous component
	    typename ExtraDelim>             // Extra delimiters to be added
  constexpr DECLAUTO _nnarySmETCompsMergeability(Tuple<MergeDelim...>,
						 Tuple<PosOfResTcsInRefTk...>,
					         IntSeq<PrevPosInts...>,
                                                 ExtraDelim)
  {
    static_assert(isIntSeq<ExtraDelim>,"ExtraDelim needs to be IntSeq");
    static_assert((isIntSeq<MergeDelim> && ...),"MergeDelim need to be IntSeq");
    static_assert((isIntSeq<PosOfResTcsInRefTk> && ...),"PosOfResTcsInRefTk need to be IntSeq");
    
    /// Number of remaining \c TensComp to be checked
    constexpr int nC=
      TupleElementType<0,Tuple<PosOfResTcsInRefTk...>>::size;
    
    static_assert(((nC==PosOfResTcsInRefTk::size) && ...),"All PosOfResTcsInRefTk must have the same size");
    
    // If we are terminating, return the final position
    if constexpr(nC==0)
      return IntSeq<ResPos>{};
    else
      {
	/// Slice with the poisition of the current component
	using CurPos=
	  IntSeq<PosOfResTcsInRefTk::first...>;
	
	/// Nested delimiters
	using Nested=
	  decltype(_nnarySmETCompsMergeability<ResPos+1>(Tuple<MergeDelim...>{},
				      Tuple<IntSeqGetAllButFirst<PosOfResTcsInRefTk>...>{},
						CurPos{},ExtraDelim{}));
	
	/// Check whether the current component is not consecutive in all Refs
	constexpr bool curNotConsecutive=
	  (((PrevPosInts!=NOT_PRESENT) and
	    (PosOfResTcsInRefTk::first!=PrevPosInts+1)) || ...);
	
	/// Check whether the current component was not originally mergeable
	constexpr bool originallyNotMergeable=
	  (MergeDelim::template has<PosOfResTcsInRefTk::first> || ...);
	
	/// Check where the current component is present
	using CurPres=
	  IntSeq<(PosOfResTcsInRefTk::first!=NOT_PRESENT)...>;
	
	/// Check where the previous component was present
	using PrevPres=
	  IntSeq<(PrevPosInts!=NOT_PRESENT)...>;
	
	/// Check it the position was extra-delimited
	constexpr bool isInExtraDelim=
	  ExtraDelim::template has<ResPos>;
	
	/// Determine if a break in mergeability is needed
	constexpr bool insBreak=
	  originallyNotMergeable
	 or
	  (not isSame<PrevPres,CurPres>)
	 or
	  isInExtraDelim
	 or
	  curNotConsecutive;
	
	/// Insert a break or not
	if constexpr(insBreak)
          return IntSeqCat<IntSeq<ResPos>,Nested>{};
	else
	  return Nested{};
      }
  }
  
  /// Determine the mergeability of a given \c TensComp
  ///
  /// A component is declared mergeable if its presence in all
  /// \c TensKind is of the same nature of the previous one (e.g. present
  /// only in one of the \c TensKind), if its position inside the
  /// \c TensKind is consecutive with previous \c TensComp, and if the
  /// component was mergeable in all \c TensKind
  template <typename RefsMD,                  // \c Tuple of \c IntSeq containing the MergeDelims for each Ref
	    typename PosOfResTcsInRefTks,     // Position of the result comps in the refs (\c Tuple of \c IntSeq)
	    typename ExtraDelims>             // Additional delmiters coming from the \c SmET
  using NnarySmETCompsMergeability=
    decltype(_nnarySmETCompsMergeability(RefsMD{},PosOfResTcsInRefTks{},IntSeqOfSameNumb<tupleSize<RefsMD>,0>{},ExtraDelims{}));
  
  /// Add an \c ExtraDelims \c IntSeq called \c ExtraDelims
#define PROVIDE_EXTRA_MERGE_DELIMS(...)			\
  /*! Additional delimiters */				\
  using ExtraDelims=					\
    __VA_ARGS__
  
  /// Add an empty \c ExtraDelims
#define NO_EXTRA_MERGE_DELIMS			\
  /*! No Additional MergeDelims is included */	\
  PROVIDE_EXTRA_MERGE_DELIMS(IntSeq<>)
  
  /// Provide \c MergeableComps delimiter according to references and extra
#define PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_REFS_AND_EXTRA		\
  PROVIDE_MERGEABLE_COMPS(/*! Using \c MergingDelimsForRefs and \c ExtraDelims to determine the mergability */, \
			  NnarySmETCompsMergeability<			\
			   Tuple<typename RemRef<_Refs>::MergeableComps...>,\
			   PosOfResTcsInRefsTk,				\
			   ExtraDelims>)
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the merged comps view of the \c I-th \c Ref
#define MERGED_COMPS_VIEW_OF_REF(I)					\
  get<I>(refs).template getMergedCompsView<TupleElementType<I,MDs>>()
  
  /// Provides a \c getMergedCompsView method, taking Is as template parameter
#define PROVIDE_NNARY_GET_MERGED_COMPS_VIEW(DESCRIPTION,...)		\
  DESCRIPTION								\
  template <typename Is>       /* IntSeq delimiting the comps groups */ \
  DECLAUTO getMergedCompsView()						\
    const								\
  {									\
    /* Check that we can merge as asked */				\
    assertMergeableWith<Is>();						\
									\
    using MDs=								\
      Tuple<InsertInOrderedIntSeq /* Insert the begin */		\
	    <0,        /* Insert 0 as begin */				\
	     IntSeqGetElsAfterAppending					\
	     <RemRef<_Refs>::Tk::nTypes,				\
	      false,   /* Omit NOT_PRESENT */				\
	      Is,							\
	      PosOfTypesNotAsserting<typename Tk::types,		\
				     typename RemRef<_Refs>::Tk::types>>, \
	     0,         /* Shift after inserting */			\
	     true>...>; /* Ignore 0 if present   */			\
    									\
    __VA_ARGS__;							\
  }									\
									\
  PROVIDE_ALSO_NON_CONST_METHOD(getMergedCompsView)
  
  /// Uses the \c representativeFunction to provide a merged view
#define PROVIDE_NNARY_GET_MERGED_COMPS_VIEW_ACCORDING_TO_REPRESENTATIVE_FUNCTION \
  /*! Calls the \c representativeFunction with the merged view of each ref */ \
  template <int...Is,     /* Ints used to call the representativeFunction  */ \
	    typename MDs> /* MergeDelims to be used                        */ \
  DECLAUTO mergedCompsViewAccordingToRepresentativeFunction(IntSeq<Is...>, \
							    MDs) const	\
  {									\
    return representativeFunction(MERGED_COMPS_VIEW_OF_REF(Is)...);	\
  }									\
  									\
  PROVIDE_ALSO_NON_CONST_METHOD(mergedCompsViewAccordingToRepresentativeFunction); \
									\
  PROVIDE_NNARY_GET_MERGED_COMPS_VIEW(/*! Uses the \c representativeFunction to provide a merged view */, \
				      return mergedCompsViewAccordingToRepresentativeFunction(IntsUpTo<NSmET>{},MDs{}))
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking n references
#define PROVIDE_NNARY_SMET_SIMPLE_CREATOR(NNARY_SMET /*!< Name of the NnarySmET */) \
  /*! Constructor taking universal reference */				\
  template <typename...SMETS,						\
	    typename=EnableIf<((isSame<Unqualified<SMETS>,Unqualified<_Refs>>) && ...)>> \
  explicit NNARY_SMET(SMETS&&...smets) : refs(forw<SMETS>(smets)...)	\
  {									\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines the check for a NnarySmET
#define STATIC_ASSERT_IS_NNARY_SMET(...)		\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(refs,__VA_ARGS__)
  
  /////////////////////////////////////////////////////////////////
  
  /// Create a simple builder with a name and a NNARY_SMET returned type
  ///
  /// \c NnarySmET cannot use SFINAE to worsen default constructors
  /// (unless packing args in \c Tuple) so the simple builder cannot
  /// be overloaded. If required, use a custom builder and use if
  /// constexpr construct inside it to route the manipulations.
#define SIMPLE_NNARY_SMET_BUILDER(BUILDER,     /*!< Name of builder function        */ \
				  NNARY_SMET)  /*!< Name of the NnarySmET to build */ \
  /*! Simple NNARY_SMET builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain NNARY_SMET getting n plain SmET    */			\
  template <typename...Ts> 	   /* Types of the SmET to get       */ \
  NNARY_SMET<Ts...> BUILDER(Ts&&...smets)    /*!< SmETs to act upon  */ \
  {									\
    return NNARY_SMET<Ts...>(forw<Ts>(smets)...);			\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Set aliasing according to the isAliasing of references
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_REFS			\
  /*! Forward aliasing check to the references */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    CRASH("Fixme");					\
    return true;					\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Provides \c Fund, \c eval and \c mergedComps according to \c representativeFunction
#define REPRESENTATIVE_FUNCTION_WINS_ALL				\
  PROVIDE_POS_OF_RES_TCS_IN_REFS;					\
									\
  PROVIDE_FUND_ACCORDING_TO_REPRESENTATIVE_FUNCTION;			\
									\
  PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_REFS_AND_EXTRA;			\
									\
  PROVIDE_NNARY_GET_MERGED_COMPS_VIEW_ACCORDING_TO_REPRESENTATIVE_FUNCTION; \
  									\
  EVAL_THROUGH_REPRESENTATIVE_FUNCTION_PASSING_COMPS_BY_NAME
  
  
  /// Implements a duplicated-call canceller
  ///
  /// Example
  /// \code
  /// Tens<TensKind<Compl>,double> cicc;
  /// conj(conj(cicc)); // returns cicc
  /// \endcode
#define CANCEL_DUPLICATED_NNARY_SMET_CALL(CALLER,     /*!< Name of builder */ \
					  NNARY_SMET) /*!< Type to un-nest */ \
  /*! Simplify CALLER(NNARY_SMET) expression */				\
  /*!                                        */				\
  /*! Returns the nested reference           */				\
  template <typename T,                                        /* Type of the expression                  */ \
	    typename RrT=RemRef<T>,                            /* T without ref attributes                */ \
	    typename Ref=typename RrT::template Ref<0>,        /* Type of the reference                   */ \
	    typename RrRef=RemRef<Ref>,                        /* Ref without ref attributes              */ \
	    bool SmETIsLvalue=isLvalue<RrT>,		       /* Detect if SmET is an lvalue             */ \
	    bool RefIsLvalue=isLvalue<RrRef>,		       /* Detect if Ref is an lvalue              */ \
	    bool RefIsStoring=isStoring<RrRef>,		       /* Detect if Ref is storing                */ \
	    bool RetByRef=RefIsStoring or	               /* Returns by val if Ref is storing, or    */ \
	    RefIsLvalue or SmETIsLvalue,	               /*   lvalue is involved         	          */ \
	    typename Ret=Conditional<RetByRef,RrRef&,RrRef>,   /* Returned type                           */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## NNARY_SMET<RrT>)>     /* Enable only for the NNARY_SMET required */ \
  Ret CALLER(T&& smet)	/*!< Quantity to un-nest   */			\
  {									\
    if constexpr(0)							\
      {									\
	constexpr bool SmETIsConst=isConst<T>;				\
	constexpr bool RefIsConst=isConst<Ref>;				\
									\
	using namespace std;						\
	cout<<"Removing duplicated call " # CALLER<<" "<<__PRETTY_FUNCTION__<<endl; \
	constexpr bool SmETIs=std::is_reference<T>::value;		\
	constexpr bool RefIs=std::is_reference<Ref>::value;		\
	cout<<" SmETIsLvalue: "<<SmETIsLvalue<<endl;			\
	cout<<" SmETIs: "<<SmETIs<<endl;				\
	cout<<" SmETIsConst: "<<SmETIsConst<<endl;			\
	cout<<" RefIsLvalue: "<<RefIsLvalue<<endl;			\
	cout<<" RefIs: "<<RefIs<<endl;					\
	cout<<" RefIsConst: "<<RefIsConst<<endl;			\
      }									\
									\
    return static_cast<Ret>(get<0>(smet.refs));				\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Defines a simple way to swap nested \c NnarySmET
  ///
  /// \todo we need to implement the same check done for
  /// \c CANCEL_DUPLICATED_NNARY_SMET_CALL
#define NNARY_SMET_GOES_INSIDE(EXT_FUN,	   /*!< External builder */	\
			       NNARY_SMET, /*!< Name of the SmET */	\
			       INT_FUN)	   /*!< Internal builder */	\
  /*! Simplify EXT_FUN(NNARY_SMET u) expression    */			\
  /*!                                              */			\
  /*! Returns INT_FUN(EXT_FUN(u.ref))              */			\
  template <typename D,                                   /* Type of the nested NNARY_SMET           */ \
	    SFINAE_ON_TEMPLATE_ARG(is ## NNARY_SMET<D>)>  /* Enable only for the NNARY_SMET required */ \
  DECLAUTO EXT_FUN(D&& smet)     /*!< NnarySmET to nest           */	\
  {									\
    return INT_FUN(EXT_FUN(get<0>(smet.refs)));				\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
}

#endif
