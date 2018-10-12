#ifndef _REL_BIND_HPP
#define _REL_BIND_HPP

/// \file RelBind.hpp
///
/// \brief Defines a class which binds a component of a SmET w.r.t another


#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <ints/IntSeqInsert.hpp>
#include <ints/IntSeqRemove.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <tens/TensClass.hpp>
#include <tens/TensComp.hpp>
#include <smet/Reference.hpp>
#include <smet/UnarySmET.hpp>

namespace SUNphi
{
  Bisogna costruire tutto suulla base di Bind

// Base type to qualify as RelBinder
  DEFINE_BASE_TYPE(RelBinder);
  
  /// Class to bind a component of a SmET
  ///
  /// The first type is the visible one, the second one is the bound
  /// (invisible) one
  template <typename _BoundToType,                                  // TensKind to which bind
	    typename _BoundType,                                    // TensKind to bind
	    typename _Ref,                                          // Type to bind
	    typename NestedTk=typename RemoveReference<_Ref>::Tk,   // Tens Kind of the bound type
	    typename NestedTypes=typename NestedTk::types>          // Types of the tensor kind
  class RelBinder :
    public BaseRelBinder,                                           // Inherit from BaseRelBinder to detect in expression
    public UnarySmET<RelBinder<_BoundToType,_BoundType,_Ref>>,      // Inherit from UnarySmET
    public ConstrainIsSmET<_Ref>,                                   // Constrain Ref to be a SmET
    public ConstrainTupleHasType<_BoundToType,NestedTypes>,         // Constrain the bound type to be in the Types of the TensKind
    public ConstrainTupleHasType<_BoundType,NestedTypes>            // Constrain the type to which to bind to be in the Types of the TensKind
  {
    /// Position inside the reference of the TensKind got by the bounder
    static constexpr int boundPos=
      posOfType<_BoundType,typename NestedTk::types>;
    
    /// Position inside the reference of the TensKind to which bind
    static constexpr int boundToPos=
      posOfType<_BoundToType,typename NestedTk::types>;
    
  public:
    
    /// Type to bind
    using BoundType=
      _BoundType;
    
    /// Type to which to bind
    using BoundToType=
      _BoundToType;
    
    PROVIDE_UNARY_SMET_REF;
    
    // Attributes
    NOT_STORING;
    ASSIGNABLE_ACCORDING_TO_REF;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// Returns the size of a component
    template <typename TC>
    int compSize() const
    {
      static_assert(not areSame<BoundType,TC>,"Cannot ask for the size of the bound component");
      return ref.template compSize<TC>();
    }
    
    /// TensorKind of the bound expression
    using Tk=typename NestedTk::template AllButType<BoundType>;
    
    // We remove at Pos, shift and insert back
    PROVIDE_MERGEABLE_COMPS(/*! We have to split at the component where we bind */,
			    InsertInOrderedIntSeq<
			    pos      // Position where to insert, same of where to remove
			    ,RemoveFromOrderedIntSeq<pos,                                       // Position where to remove
			                             typename Unqualified<Ref>::MergeableComps, // Nested components
			                             -1>                                        // Shift back by 1 after removal
			    ,0       // Shift 0 after insertion
			    ,true>); // Ignore if already present
    
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Insert the position of bind component shifting by 1 afterwards */,
				  using NestedIs=InsertInOrderedIntSeq<
				    pos,                 // Position where to insert
				    Is,                  // External delimiters
				    1>;                  // Shift 1 after insertion
				  auto refMerged=ref.template mergedComps<NestedIs>();
				  return Binder<TG,decltype(refMerged)>(std::move(refMerged),id));
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER)		\
    /*! QUALIFIER evaluator for Binder                               */	\
    /*!                                                              */	\
    /*! Internal Evaluator, inserting the id at the correct          */	\
    /*! position in the list of args. Check on type B is omitted, as */	\
    /*! the function is called only from an already checked smet     */ \
    template <typename...Args, /* Type of the arguments */		\
	      int...Head,      /* Position of the first set of args, before insertion */ \
	      int...Tail>      /* Position of the second set of args, after insertion */ \
    DECLAUTO binder_internal_eval(IntSeq<Head...>,              /*!< List of position of components before id */ \
				  IntSeq<Tail...>,              /*!< List of position of components after id  */ \
				  const Tuple<Args...>& targs)  /*!< Components to get                        */ \
      QUALIFIER								\
    {									\
      return ref.eval(get<Head>(targs)...,				\
		      id,						\
		      get<Tail>(targs)...);				\
    }									\
									\
    /*! Evaluator, external interface                                */	\
    /*!                                                              */	\
    /*! Pass to the internal implementation the integer sequence     */	\
    /*! needed to deal properly with the insertion of the arg in the */	\
    /*! correct position */						\
    template <typename...Args>           /* Type of the arguments    */	\
    DECLAUTO eval(const Args&...args)    /*!< Components to get      */	\
      QUALIFIER								\
    {									\
      STATIC_ASSERT_ARE_N_TYPES(Tk::nTypes,args);			\
									\
      using Head=IntsUpTo<pos>;						\
									\
      if(0)								\
	{								\
	  using namespace std;						\
	  cout<<" evaluating binder of component "<<Tg::name();		\
	  cout<<" position in NestedTk: "<<pos;				\
	  cout<<" of "<<NestedTk::nTypes<<" types,";			\
	  cout<<" id: "<<id;						\
	  cout<<endl;							\
	}								\
      									\
      using Tail=typename IntsUpTo<Tk::nTypes-pos>::template Add<pos>; \
									\
      return binder_internal_eval(Head{},				\
      				  Tail{},				\
      				  std::forward_as_tuple(args...));	\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR
    
    PROVIDE_UNARY_SMET_ASSIGNEMENT_OPERATOR(Binder);
    
    /// Constructor taking a universal reference and the id
    ///
    /// \todo add check on SMET
    template <typename SMET,						\
	      typename=EnableIf<isSame<Unqualified<SMET>,Unqualified<Ref>>>> \
    explicit Binder(SMET&& smet, ///< Reference to bind
		    int id)    ///< Component to get
      : ref(forw<SMET>(smet)),id(id)
    {
#ifdef DEBUG_BINDER
      using namespace std;
      cout<<"Constructing binder "<<this<<", type "<<TG::name()<<", component: "<<id<<endl;
#endif
    }
    
    /// Destructor
    ~Binder()
    {
#ifdef DEBUG_BINDER
      using namespace std;
      cout<<"Destroying binder "<<this<<endl;
#endif
    }
    
  };
  
  /// Bind the \c id component of type \c Tg from expression \c ref
  ///
  /// Returns a plain binder getting from an unbind expression. Checks
  /// demanded to Binder.
  template <typename _Tg,                       // Type to get
	    typename SMET,                       // Type to bind, deduced from argument
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  DECLAUTO bind(SMET&& smet,                     ///< Quantity to bind to
		const int id,                  ///< Entry of the component to bind
		SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    using Tg=CompOrTwinned<_Tg,SMET>;
    
#ifdef DEBUG_BINDER
    using namespace std;
    cout<<"Constructing a binder for type "<<Tg::name()<<" , storage: "<<getStor(ref)._v<<endl;
#endif
    
    // Build the binder
    Binder<Tg,SMET> b(forw<SMET>(smet),id);
    
    // Get the TensKind
    using Tk=typename Unqualified<SMET>::Tk;
    // Count the free components of SmET
    constexpr int N=Tk::nTypes;
    // Check if fully bound
    constexpr bool FullyBound=(N==1);
    
    // Evaluate if fully bound
    if constexpr(FullyBound)
       return b.eval();
    else
      return b;
  }
  
  // /// Bind the \c id component of type \c Tg from expression \c ref
  // ///
  // /// Nested binder getting an already bound expression, swapping the
  // /// inner and outer types and components if needed.
  // ///
  // /// \code
  // /// Tens<TensKind<Compl,RwSpin>,double> cicc;
  // /// spin(reim(cicc,0),1);
  // /// reim(spin(cicc,1),0);
  // /// \endcode
  // ///
  // /// This might be not enough to ensure proper evaluation if the
  // /// nesting occurs through a wrapper, as in the following example
  // ///
  // /// \code
  // /// spin(wrap(reim(cicc,0)),1);
  // /// reim(wrap(spin(cicc,1)),0);
  // /// \endcode
  // template <typename _Tg,                        // Type to get
  // 	    typename InNestedTg,                 //Type got by the nested bounder
  // 	    typename InNestedRef>                // Type of the nested binder
  // DECLAUTO bind(const Binder<InNestedTg,InNestedRef>& nb,                          ///< Binder to rebind
  // 		const int id)                    ///< Component to get
  // {
  //   // True component searched
  //   using Tg=CompOrTwinned<_Tg,Binder<InNestedTg,InNestedRef>>;
  //   // Tensor Kind of input nested binder
  //   using InNestedRefTk=typename RemoveReference<InNestedRef>::Tk;
  //   // Types of the Tensor Kind of nested bounder
  //   using NestedTypes=typename InNestedRefTk::types;
  //   // Position inside the nested reference of the type got by the nested bounder
  //   constexpr int InNestedNestedTgPos=posOfType<InNestedTg,NestedTypes>;
  //   // Position inside the nested reference of the type to get
  //   static constexpr int NestedTgPos=posOfType<Tg,NestedTypes>;
  //   // Keep note of whether we need to swap
  //   constexpr bool swap=(NestedTgPos>InNestedNestedTgPos);
  //   // Type got by the output nested binder
  //   using OutNestedTg=Conditional<swap,Tg,InNestedTg>;
  //   // Type got by the output binder
  //   using OutTg=Conditional<swap,InNestedTg,Tg>;
  //   // Nested component
  //   const int nestedId=nb.id;
  //   // Out external component
  //   const int outNestedId=(swap?id:nestedId);
  //   // Out component
  //   const int outId=(swap?nestedId:id);
  //   // Output Nested binder
  //   auto outNestedBinder=bind<OutNestedTg>(nb.ref,outNestedId);
  //   // Type of the output nested binder
  //   using OutNestedBinder=decltype(outNestedBinder);
    
  //   //cout<<"Constructing a nested binder for type "<<Tg::name()<<", internal binder gets: "<<InNestedTg::name()<<", swap: "<<swap<<endl;
  //   // cout<<"OutTg: "<<OutTg::name()<<" "<<endl;
  //   return evalIfFullyBound(Binder<OutTg,OutNestedBinder>(outNestedBinder,outId));
  // }
  
  // Check that a test Binder is a UnarySmET
  STATIC_ASSERT_IS_UNARY_SMET(Binder<TensComp<double,1>,
			      Tens<TensKind<TensComp<double,1>>,double>>);
}

#endif
