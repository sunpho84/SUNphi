#ifndef _BIND_HPP
#define _BIND_HPP

/// \file Bind.hpp
///
/// \brief Defines a class which binds a component of a TEx

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <tens/TensClass.hpp>
#include <tex/Reference.hpp>
#include <tex/UnaryTEx.hpp>

namespace SUNphi
{
  // Base type to qualify as Binder
  DEFINE_BASE_TYPE(Binder);
  
  /// Class to bind a component of a TEx
  template <typename TG,                                 // Type to get
	    typename _B,                                  // Type to bind
	    typename TK=typename RemoveReference<_B>::Tk, // Tens Kind of the bound type
	    typename TK_TYPES=typename TK::types>        // Types of the tensor kind
  class Binder :
    public BaseBinder,                          // Inherit from BaseBinderer to detect in expression
    public UnaryTEx<Binder<TG,_B>>,              // Inherit from UnaryTEx
    public ConstrainIsTEx<_B>,                   // Constrain B to be a TEx
    public ConstrainIsTensKind<TK>,             // Constrain type TK to be a TensKind
    public ConstrainTupleHasType<TG,TK_TYPES>   // Constrain TG to be in the Types of the TensKind
  {
    /// Nested type Tensor Kind
    using NestedTk=TK;
    
    // Position inside the reference of the type got by the bounder
    static constexpr int pos=posOfType<TG,typename NestedTk::types>;
    
  public:
    
    /// Type to bind
    using B=_B;
    
    /// Type to get
    using Tg=TG;
    
    PROVIDE_UNARY_TEX_REF(B);
    
    // Attributes
    NOT_STORING;
    ASSIGNABLE_ACCORDING_TO_REF;
    FORWARD_IS_ALIASING_TO_REF;
    
    /// Index to get
    const int id;
    
    /// TensorKind of the bound expression
    using Tk=typename NestedTk::template AllButType<TG>;
    
    /// Provides the evaluator with or without const qualifier
#define PROVIDE_CONST_OR_NON_CONST_EVALUATOR(QUALIFIER)			\
    /*! QUALIFIER Internal Evaluator                              */	\
    /*!                                                           */	\
    /*! Insert the id at the correct position in the list of args */	\
    template <class...Args,						\
	      int...Head,						\
	      int...Tail>						\
    friend DECLAUTO _eval(QUALIFIER Binder& binder,      /*!< binder to eval                           */ \
			  IntSeq<Head...>,               /*!< list of position of components before id */ \
			  IntSeq<Tail...>,               /*!< list of position of components after id  */ \
			  const Tuple<Args...>& targs)   /*!< components to get                        */ \
    {									\
      return eval(binder.ref,						\
		  get<Head>(targs)...,					\
		  binder.id,						\
		  get<Tail>(targs)...);					\
    }									\
    									\
    /*! QUALIFIER Evaluator                                          */	\
    /*!                                                              */ \
    /*! Pass to the internal implementation the integer sequence     */	\
    /*! needed to deal properly with the insertion of the arg in the */	\
    /*! correct position                                             */	\
    template <class...Args>						\
    friend DECLAUTO eval(QUALIFIER Binder& binder,  /*!< binder to eval    */ \
			 const Args&...args)        /*!< components to get */ \
    {									\
      STATIC_ASSERT_ARE_N_TYPES(TK::nTypes-1,args);			\
									\
      using Head=IntsUpTo<pos>;						\
      using Tail=typename IntsUpTo<TK::nTypes-1-pos>::template Add<pos>; \
									\
      return _eval(binder,						\
		   Head{},						\
		   Tail{},						\
		   std::forward_as_tuple(args...));			\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    PROVIDE_CONST_OR_NON_CONST_EVALUATOR();
    PROVIDE_CONST_OR_NON_CONST_EVALUATOR(const);
    
#undef PROVIDE_CONST_OR_NON_CONST_EVALUATOR
    
    /// Constructor taking a universal reference and the id
    explicit Binder(B&& tex, ///< Reference to bind
		    int id)  ///< Component to get
      : ref(tex),id(id)
    {
    }
  };
  
  // Check that a test Binder is a UnaryTEx
  STATIC_ASSERT_IS_UNARY_TEX(Binder<TensComp<double,1>,Tens<TensKind<TensComp<double,1>>,double>>);
  
  /// Bind the \c id component of type \c Tg from expression \c ref
  ///
  /// Returns a plain binder getting from an unbind expression. Checks
  /// demanded to Binder
  template <typename Tg,                        // Type to get
	    typename Tb>                        // Type to bind, deduced from argument
  DECLAUTO bind(Tb&& ref,               ///< Quantity to bind to
		const int id)           ///< Entry of the component to bind
  {
    //cout<<"Constructing a binder for type "<<Tg::name()<<endl;
    return Binder<Tg,Tb>(forw<Tb>(ref),id);
  }
  
  /// Bind the \c id component of type \c Tg from expression \c ref
  ///
  /// Nested binder getting an already bound expression, swapping the
  /// inner and outer types and components if needed.
  ///
  /// \code
  /// Tens<TensKind<Compl,RwSpin>,double> cicc;
  /// spin(reim(cicc,0),1);
  /// reim(spin(cicc,1),0);
  /// \endcode
  ///
  /// This might be not enough to ensure proper evaluation if the
  /// nesting occurs through a wrapper, as in the following example
  ///
  /// \code
  /// spin(wrap(reim(cicc,0)),1);
  /// reim(wrap(spin(cicc,1)),0);
  /// \endcode
  template <typename Tg,                        // Type to get
	    typename InNested,                  // Type referred from the nested bounder
	    typename InNestedTg>                // Type got by the nested bounder
  DECLAUTO bind(Binder<InNestedTg,InNested>&& ref,      ///< Quantity to bind
		      const int id)                     ///< Component to get
  {
    // Tensor Kind of input nested binder
    using InNestedTk=typename RemoveReference<InNested>::Tk;
    // Types of the Tensor Kind of nested bounder
    using NestedTypes=typename InNestedTk::types;
    // Position inside the nested reference of the type got by the nested bounder
    constexpr int InNestedNestedTgPos=posOfType<InNestedTg,NestedTypes>;
    // Position inside the nested reference of the type to get
    static constexpr int NestedTgPos=posOfType<Tg,NestedTypes>;
    // Keep note of whether we need to swap
    constexpr bool swap=(NestedTgPos>InNestedNestedTgPos);
    // Type got by the output nested binder
    using OutNestedTg=Conditional<swap,Tg,InNestedTg>;
    // Type got by the output binder
    using OutTg=Conditional<swap,InNestedTg,Tg>;
    // Nested component
    const int nestedId=ref.id;
    // Out external component
    const int outNestedId=(swap?id:nestedId);
    // Out component
    const int outId=(swap?nestedId:id);
    // Output Nested binder
    auto outNestedBinder=bind<OutNestedTg>(forw<InNested>(ref.ref),outNestedId);
    // Type of the output nested binder
    using OutNestedBinder=decltype(outNestedBinder);
    
    //cout<<"Constructing a nested binder for type "<<Tg::name()<<", internal binder gets: "<<InNestedTg::name()<<", swap: "<<swap<<endl;
    // cout<<"OutTg: "<<OutTg::name()<<" "<<endl;
    return Binder<OutTg,OutNestedBinder>(std::move(outNestedBinder),outId);
  }
  
  /// Defines a Binder named NAME for type TG
#define DEFINE_NAMED_BINDER(TG,NAME)					\
  /*! Get a reference to the \c TG component \c id of \c ref */		\
  template <typename T>	      /* Type of the bound expression */	\
  DECLAUTO NAME(T&& ref,      /*!< Quantity to be bind  */		\
		const int id) /*!< Component to bind    */		\
  {									\
    return bind<TG>(forw<T>(ref),id);					\
  }
  
  /// Defines a Binder named NAME for type RwTG or CnTG
#define DEFINE_NAMED_RW_OR_COL_BINDER(TG,NAME)				\
  /*! Returns a binder to the only Rw TG or Cn TG type available */	\
  template <typename T>       /* Type of the bound expression */	\
  DECLAUTO NAME(T&& ref,      /*!< Quantity to be bound       */	\
		const int id) /*!< Component to bind          */	\
  {									\
    /* TensKind of binding expression */				\
    using Tk=typename RemoveReference<T>::Tk;				\
    									\
    /* Tuple containing all Tk types */					\
    using Tp=typename Tk::types;					\
    									\
    /* Check if row type is available */				\
    constexpr bool hasRw=tupleHasType<Rw ## TG,Tp>;			\
    									\
    /* Check if column type is available */				\
    constexpr bool hasCn=tupleHasType<Cn ## TG,Tp>;			\
    									\
    /* Check that not more than one type is available */		\
    constexpr bool hasOnlyOneType=(hasRw+hasCn<2);			\
    static_assert(hasOnlyOneType,"Both types Rw and Cn identfied!");	\
    									\
    /* Check that at least one type is available */			\
    constexpr bool hasAtLeastOneType=(hasRw+hasCn>0);			\
    static_assert(hasAtLeastOneType,"No types Rw and Cn identfied!");	\
    									\
    /* Identifies the type to return, on the basis of the check above */ \
    using Ret=Conditional<hasRw,Rw ## TG,Cn ## TG>;			\
    									\
    return bind<Ret>(forw<T>(ref),id);					\
  }
}

#endif
