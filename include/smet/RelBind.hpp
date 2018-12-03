#ifndef _REL_BIND_HPP
#define _REL_BIND_HPP

/// \file RelBind.hpp
///
/// \brief Defines a class which binds a component of a SmET w.r.t another

#include <ints/IntSeqInsert.hpp>
#include <ints/IntSeqRemove.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <tens/TensClass.hpp>
#include <tens/TensComp.hpp>
#include <smet/Bind.hpp>

namespace SUNphi
{
  /// Define the prototype for \c relBind function
#define REL_BIND_PROTOTYPE						\
  template <typename _BoundType,                  /* TensKind to bind                    */ \
	    typename _BoundToType,                /* TensKind to which bind              */ \
	    typename SMET,                        /* Type to bind, deduced from argument */ \
	    typename _Ad,                         /* Adapter of the bound component      */ \
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>     		\
  DECLAUTO relBind(SMET&& smet,                   /*!< Quantity to bind to */ \
		   _Ad adapter,                   /*!< Adapting function   */ \
		   SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  
  // Forward definition of relBind function, needed because used from the class
  REL_BIND_PROTOTYPE;
  
  // Base type to qualify as RelBinder
  DEFINE_BASE_TYPE(RelBinder);
  
  /// Class to bind a component of a SmET
  ///
  /// The first type is the bound (invisible) one, the second one is
  /// the free (visible) one
  template <typename _BoundType,                                        // TensKind to bind
	    typename _BoundToType,                                      // TensKind to which bind
	    typename _Ad,                                               // Adapter of the bound component
	    typename..._Refs>                                           // Types of nested \c SMeT
  class RelBinder :
    public BaseRelBinder,                                               // Inherit from \c BaseRelBinder to detect in expression
    public NnarySmET<RelBinder<_BoundType,_BoundToType,_Ad,_Refs...>>,  // Inherit from \c NnarySmET
    public ConstrainAreSmETs<_Refs...>                                  // Constrain all \c Refs to be \c SmET
  {
  public:
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(1);
    
    /// Tens Kind of the bound type
    using NestedTk=
      typename RemRef<Ref<0>>::Tk;
    
    /// Types of the tensor kind
    using NestedTypes=
      typename NestedTk::types;
    
    // Constrain the type to which to bind to be in the Types of the TensKind
    STATIC_ASSERT_TUPLE_HAS_TYPE(_BoundType,NestedTypes);
    
    // Constrain the bound type to be in the Types of the TensKind
    STATIC_ASSERT_TUPLE_HAS_TYPE(_BoundToType,NestedTypes);
    
    /// Type to bind
    using BoundType=
      _BoundType;
    
    /// Type to which to bind
    using BoundToType=
      _BoundToType;
    
  private:
    
    /// Position inside the reference of the TensKind got by the bounder
    static constexpr int boundPos=
      posOfType<_BoundType,typename NestedTk::types>;
    
    /// Position inside the reference of the TensKind to which to bind
    static constexpr int boundToPos=
      posOfType<_BoundToType,typename NestedTk::types>;
    
    /// Adapter, used to remap the boundTo component
    ///
    /// The pointer is needed to allow non-member function to be
    /// passed as adaptor, see:
    /// https://stackoverflow.com/questions/13233213/can-a-function-type-be-a-class-template-parameter
    AddPointerIfFunction<_Ad> adapter;
    
    /// Dummy type, in which the _BoundType is absolutely bound
    using absBinder=Binder<_BoundType,Ref<0>>;
    
  public:
    
    /// TensorKind of the bound expression
    PROVIDE_TK(typename NestedTk::template AllButType<BoundType>);
    
    /// Fundamental type
    SAME_FUND_AS_REF(0);
    
    // Attributes
    NOT_STORING;
    AS_ASSIGNABLE_AS_REF(0);
    
    FORWARD_IS_ALIASING_TO_REFS;
    
    PROVIDE_SIMPLE_NNARY_COMP_SIZE;
    
    /// Position inside the external TensKind to which to bind
    static constexpr int extBoundToPos=
      posOfType<_BoundToType,typename Tk::types>;
    
    PROVIDE_EXTRA_MERGE_DELIMS(IntSeq<extBoundToPos,extBoundToPos+1>);
    
    PROVIDE_POS_OF_RES_TCS_IN_REFS;
    
    PROVIDE_MERGEABLE_COMPS_ACCORDING_TO_REFS_AND_EXTRA;
    
    PROVIDE_NNARY_GET_MERGED_COMPS_VIEW(/*! Insert the position of bound component shifting by 1 afterwards */,
					return relBind<_BoundType,_BoundToType>(MERGED_COMPS_VIEW_OF_REF(0),adapter));
    
    /// Evaluator for \c RelBinder
    ///
    /// Internal Evaluator, inserting the id at the correct
    /// position in the list of args. Check on type B is omitted, as
    /// the function is called only from an already checked smet
    template <typename...Args, // Type of the arguments
	      typename Id,     // Type of the argument of the bound component
	      int...Head,      // Position of the first set of args, before insertion
	      int...Tail>      // Position of the second set of args, after insertion
    DECLAUTO relBinderInternalEval(IntSeq<Head...>,              ///< List of position of components before id
				   IntSeq<Tail...>,              ///< List of position of components after id
				   const Id& id,	            ///< Component to insert
				   const Tuple<Args...>& targs)  ///< Components to get
      const
    {
      
      return get<0>(refs).eval(get<Head>(targs)...,
			       id,
			       get<Tail>(targs)...);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(relBinderInternalEval);
    
    /// Evaluator, external interface
    ///
    /// Pass to the internal implementation the integer sequence
    /// needed to deal properly with the insertion of the arg in the
    /// correct position
    template <typename...Args>           // Type of the arguments
    DECLAUTO eval(const Args&...args)    ///< Components to get
      const
    {
      STATIC_ASSERT_ARE_N_TYPES(Tk::nTypes,args);
      
      using Head=IntsUpTo<boundPos>;
      
      /// Value of the boundTo component
      const auto boundToId=
	get<extBoundToPos>(Tuple<Args...>(args...));
      
      /// Value of the bound component
      const decltype(boundToId) boundId=
	adapter(boundToId);
      
      if(0)
	{
	  using namespace std;
	  cout<<" evaluating rel_binder of component "<<_BoundType::name();
	  cout<<" bound to component "<<_BoundToType::name()<<",";
	  cout<<" position where to read the boun-tod component: "<<extBoundToPos;
	  cout<<" bound to component "<<_BoundToType::name();
	  cout<<" of "<<NestedTk::nTypes<<" types,";
	  cout<<" boundToId: "<<boundToId;
	  cout<<" boundId: "<<boundId;
	  cout<<endl;
	}
      
      using Tail=
	RangeSeq<boundPos,1,Tk::nTypes>;
      
      return relBinderInternalEval(Head{},
				   Tail{},
				   boundId,
				   std::forward_as_tuple(args...));
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(eval);
    
    PROVIDE_SMET_ASSIGNEMENT_OPERATOR(RelBinder);
    
    /// Constructor taking a universal reference and the id
    ///
    /// \todo add check on SMET
    template <typename SMET,
	      typename=EnableIf<isSame<Unqualified<SMET>,Unqualified<Ref<0>>>>>
    explicit RelBinder(SMET&& smet,          ///< Reference to bind
		       _Ad&& adapter)        ///< Adapting function
      : refs(std::forward_as_tuple(smet)),adapter(forw<_Ad>(adapter))
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
  REL_BIND_PROTOTYPE
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    
    /// Check that the two components are not twinned of each other
    constexpr bool areTwinnedOfEachOther=
      isSame<TwinCompOf<_BoundToType>,_BoundType>;
    
    using BoundType=
      Conditional<areTwinnedOfEachOther,_BoundType,CompOrTwinned<_BoundType,SMET>>;
    using BoundToType=
      Conditional<areTwinnedOfEachOther,_BoundToType,CompOrTwinned<_BoundToType,SMET>>;
    
#ifdef DEBUG_REL_BINDER
    using namespace std;
    cout<<"Constructing a rel binder for type "<<BoundType::name()<<" to type:"<<BoundToType::name()<<" , storage: "<<getStor(smet)._v<<endl;
#endif
    
    // Build the relative binder. The \c decltype is needed to allow
    // non-member function to be passd as adapter, see
    // https://stackoverflow.com/questions/13233213/can-a-function-type-be-a-class-template-parameter
    RelBinder<BoundType,BoundToType,decltype(adapter),SMET> b(forw<SMET>(smet),forw<_Ad>(adapter));
    
    return b;
  }
  
#undef REL_BIND_PROTOTYPE
  
  /// Internal checks
  namespace _RelBindInternalChecks
  {
    DEFINE_TENS_COMP(tc1,Tc1,NTC1,1);
    
    DEFINE_TENS_COMP(tc2,Tc2,NTC2,2);
    
    /// TensorKind of the expression
    using Tk=
      TensKind<Tc1,Tc2>;
    
    /// Tensor class of the expression
    using T=
      Tens<Tk,double>;
    
    /// Function returning the argument
    template <typename T>
    T wire(T t) ///< Argument to be returned
    {
      return t;
    }
    
    // Check that a test Binder is a NnarySmET
    STATIC_ASSERT_IS_NNARY_SMET(RelBinder<Tc1,Tc2,
				decltype(wire<int>),T>);
  }
}

#endif
