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
#include <smet/Bind.hpp>

namespace SUNphi
{
  // Base type to qualify as RelBinder
  DEFINE_BASE_TYPE(RelBinder);
  
  /// Class to bind a component of a SmET
  ///
  /// The first type is the visible one, the second one is the bound
  /// (invisible) one
  template <typename _BoundToType,                                  // TensKind to which bind
	    typename _BoundType,                                    // TensKind to bind
	    typename _Ref,                                          // Type of nested SMeT
	    typename _Ad,                                           // Adapter of the bound component
	    typename NestedTk=typename RemoveReference<_Ref>::Tk,   // Tens Kind of the bound type
	    typename NestedTypes=typename NestedTk::types>          // Types of the tensor kind
  class RelBinder :
    public BaseRelBinder,                                           // Inherit from BaseRelBinder to detect in expression
    public UnarySmET<RelBinder<_BoundToType,_BoundType,_Ref,_Ad>>,  // Inherit from UnarySmET
    public ConstrainIsSmET<_Ref>,                                   // Constrain Ref to be a SmET
    public ConstrainTupleHasType<_BoundToType,NestedTypes>,         // Constrain the bound type to be in the Types of the TensKind
    public ConstrainTupleHasType<_BoundType,NestedTypes>            // Constrain the type to which to bind to be in the Types of the TensKind
  {
    /// Position inside the reference of the TensKind got by the bounder
    static constexpr int boundPos=
      posOfType<_BoundType,typename NestedTk::types>;
    
    /// Position inside the reference of the TensKind to which to bind
    static constexpr int boundToPos=
      posOfType<_BoundToType,typename NestedTk::types>;
    
    /// Dummy type, in which the _BoundType is absolutely bound
    using absBinder=Binder<_BoundType,_Ref>;
    
    /// Adapter, used to remap the boundTo component
    const _Ad adapter;
    
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
    
    // We remove at boundPos, shift and insert back
    PROVIDE_MERGEABLE_COMPS(/*! We have to split at the component where we bind */,
			    InsertInOrderedIntSeq<
			    boundToPos,                                    // Position where to insert, same of where to remove
			      RemoveFromOrderedIntSeq<
			        boundToPos,                                // Position where to remove
			        typename absBinder::MergeableComps,        // Component mergeable if _BoundToType was fixd
			        -1>,                                       // Shift back by 1 after removal
			      0,       // Shift 0 after insertion
			    true>);    // Ignore if already present
    
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Insert the position of bind component shifting by 1 afterwards */,
				  using NestedIs=InsertInOrderedIntSeq<
				    boundToPos,          // Position where to insert
				    Is,                  // External delimiters
				    1>;                  // Shift 1 after insertion
				  auto refMerged=ref.template mergedComps<NestedIs>();
				  return RelBinder<_BoundToType,_BoundType,_Ad,decltype(refMerged)>(std::move(refMerged),adapter));
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(QUALIFIER)		\
    /*! QUALIFIER evaluator for RelBinder                            */ \
    /*!                                                              */	\
    /*! Internal Evaluator, inserting the id at the correct          */	\
    /*! position in the list of args. Check on type B is omitted, as */	\
    /*! the function is called only from an already checked smet     */ \
    template <typename...Args, /* Type of the arguments */		\
	      typename Id,     /* Type of the argument of the bound component         */ \
	      int...Head,      /* Position of the first set of args, before insertion */ \
	      int...Tail>      /* Position of the second set of args, after insertion */ \
    DECLAUTO rel_binder_internal_eval(IntSeq<Head...>,              /*!< List of position of components before id */ \
				      IntSeq<Tail...>,              /*!< List of position of components after id  */ \
				      const Id& id,	            /*!< Component to insert                      */ \
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
      using Head=IntsUpTo<boundPos>;					\
									\
      /*! Subtract 1 position if the bound component comes before */	\
      constexpr int readPos=						\
	(boundToPos>boundPos)?(boundToPos-1):boundToPos;		\
									\
      /*! Value of the boundTo component                             */ \
      const auto boundToId=						\
	get<readPos>(Tuple<Args...>(args...));				\
									\
      /*! Value of the bound component                               */	\
      const decltype(boundToId) boundId=adapter(boundToId);		\
									\
      if(0)								\
	{								\
	  using namespace std;						\
	  cout<<" evaluating rel_binder of component "<<_BoundType::name(); \
	  cout<<" bound to component "<<_BoundToType::name()<<",";	\
	  cout<<" position where to insert the bound component: "<<readPos; \
	  cout<<" bound to component "<<_BoundToType::name();		\
	  cout<<" of "<<NestedTk::nTypes<<" types,";			\
	  cout<<" boundToId: "<<boundToId;				\
	  cout<<" boundId: "<<boundId;					\
	  cout<<endl;							\
	}								\
      									\
      using Tail=RangeSeq<boundPos,1,Tk::nTypes>;			\
									\
      return rel_binder_internal_eval(Head{},				\
				      Tail{},				\
				      boundId,				\
				      std::forward_as_tuple(args...));	\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_CONST_OR_NOT_DEFAULT_EVALUATOR
    
    PROVIDE_UNARY_SMET_ASSIGNEMENT_OPERATOR(RelBinder);
    
    /// Constructor taking a universal reference and the id
    ///
    /// \todo add check on SMET
    template <typename SMET,						\
	      typename=EnableIf<isSame<Unqualified<SMET>,Unqualified<Ref>>>> \
    explicit RelBinder(SMET&& smet,          ///< Reference to bind
		       _Ad&& adapter)        ///< Adapting function
      : adapter(forw<_Ad>(adapter)),ref(forw<SMET>(smet))
    {
#ifdef DEBUG_REL_BINDER
      using namespace std;
      cout<<"Constructing relative binder "<<this<<", bound type "<<_BoundType::name()<<", bound to type "<<_BoundToType::name()<<endl;
#endif
    }
    
    /// Destructor
    ~RelBinder()
    {
#ifdef DEBUG_REL_BINDER
      using namespace std;
      cout<<"Destroying relative binder "<<this<<endl;
#endif
    }
    
  };
  
  /// Bind the component of type \c _BoundType to \c _BoundToType
  ///
  /// Returns a relative binder getting from an unbind expression.
  /// Checks demanded to RelBinder
  template <typename _BoundToType,                        // TensKind to which bind
	    typename _BoundType,                          // TensKind to bind
	    typename SMET,                                // Type to bind, deduced from argument
	    typename _Ad,                                 // Adapter of the bound component
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  DECLAUTO relBind(SMET&& smet,                   ///< Quantity to bind to
		   _Ad&& adapter,                 ///< Adapting function
		   SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    /// Check that the two components are not twinned of each other
    constexpr bool areTwinnedOfEachOther=
      isSame<TwinCompOf<_BoundToType>,_BoundType>;
    
    using BoundToType=
      Conditional<areTwinnedOfEachOther,_BoundToType,CompOrTwinned<_BoundToType,SMET>>;
    using BoundType=
      Conditional<areTwinnedOfEachOther,_BoundType,CompOrTwinned<_BoundType,SMET>>;
    
#ifdef DEBUG_REL_BINDER
    using namespace std;
    cout<<"Constructing a rel binder for type "<<BoundType::name()<<" to type:"<<BoundToType::name()<<" , storage: "<<getStor(smet)._v<<endl;
#endif
    
    // Build the relative binder
    RelBinder<BoundToType,BoundType,SMET,_Ad> b(forw<SMET>(smet),forw<_Ad>(adapter));
    
    return b;
  }
}

#endif
