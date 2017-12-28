#ifndef _BIND_HPP
#define _BIND_HPP

/// \file Bind.hpp
///
/// \brief Defines a class which binds a component of a TEx

#include <tens/TensClass.hpp>
#include <tex/BaseTEx.hpp>

namespace SUNphi
{
  /// Class to bind a component of a TEx
  ///
  /// The creator should accepts any type which qualifies as a TEx (TO
  /// BE FIXED?)
  template <typename TG, // Type to get
	    typename B,  // Type to bind
	    typename TK=typename std::remove_reference_t<B>::Tk, // Tens Kind of the bind type
	    typename TK_TYPES=typename TK::types>                // Types of the tensor kind
  class Binder :
    public TEx<Binder<TG,B>>,
    public ConstrainIsTensKind<TK>,             // Constrain type TK to be a TensKind
    public ConstrainTupleHasType<TG,TK_TYPES>   // Constrain TG to be in the Types of the TensKind
  {
    /// Nested type Tensor Kind
    using NestedTk=TK;
    
  public:
    
    /// Reference to the object from which to get
    Conditional<RemoveReference<B>::isStoring,B&,B> ref;
    
    /// Index to get
    const int id;
    
    /// Returns whether this TEx is storing
    static constexpr bool isStoring=false;
    
    /// TensorKind of the bound expression
    using Tk=typename NestedTk::template AllButType<TG>;
    
    // using InnerTypes=typename NestedTk::template AllAfterType<TG>;
    
    // using OuterTypes=typename NestedTk::template AllBeforeType<TG>;
    
    /// Evaluator
    template <class...Args>
    friend decltype(auto) eval(Binder& binder,const Args&...args)
    {
      return eval(binder.ref,std::forward<const Args>(args)...,binder.id);
    }
    
    /// Evaluator returning const
    template <class...Args>
    friend decltype(auto) eval(const Binder& binder,const Args&...args)
    {
      return eval(binder.ref,std::forward<const Args>(args)...,binder.id);
    }
    
    /// Constructor taking a universal reference (hopefully)
    Binder(B&& ref,int id) : ref(ref),id(id)
    {
    }
    
    // /// Constructor taking lvalue
    // Binder(B& ref,int id) : ref(ref),id(id)
    // {
    // }
    
  };
  
  /// Bind the \c id component of type \c Tg from expression \c ref
  ///
  /// Plain binder getting an unbind expression
  template <typename Tg,                         // Type to get
	    typename Tk,                         // TensKind of the ref, deduced from argument
	    typename=ConstrainIsTensKind<Tk>,   // Force ref to be a TensKind
	    typename=ConstrainIsTensComp<Tg>>   // Force binding type to be a TensComp
  auto bind(Tk&& ref,                     ///< Quantity to bind to
	    const int id)                 ///< Entry of the component to bind
  {
    //cout<<"Constructing a binder for type "<<Tg::name()<<endl;
    return Binder<Tg,Tk>(std::forward<Tk>(ref),id);
  }
  
  /// Bind the \c id component of type \c Tg from expression \c ref
  ///
  /// Nested binder getting an already bound expression, swapping the
  /// inner and outer types and components if needed.
  template <typename Tg,                        // Type to get
	    typename InNested,                  // Type referred from the nested bounder
	    typename InNestedTg,                // Type got by the nested bounder
	    typename=ConstrainIsTensComp<Tg>>  // Force binding type to be a TensComp
  auto bind(Binder<InNestedTg,InNested>&& ref,      ///< Quantity to bind
	    const int id)                           ///< Component to get
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
    auto outNestedBinder=bind<OutNestedTg>(std::forward<InNested>(ref.ref),outNestedId);
    // Type of the output nested binder
    using OutNestedBinder=decltype(outNestedBinder);
    
    //cout<<"Constructing a nested binder for type "<<Tg::name()<<", internal binder gets: "<<InNestedTg::name()<<", swap: "<<swap<<endl;
    // cout<<"OutTg: "<<OutTg::name()<<" "<<endl;
    return Binder<OutTg,OutNestedBinder>(std::move(outNestedBinder),outId);
  }
  
  /// Defines a Binder named NAME for type TG
#define DEFINE_NAMED_BINDER(TG,NAME)					\
  /*! Get a reference to the \c TG component \c id of \c ref */		\
  template <class T>							\
  auto NAME(T&& ref,     /*!< Quantity to be bound */			\
	    const int id) /*!< Compinent to bind    */			\
  {									\
    return bind<TG>(std::forward<T>(ref),id);				\
  }
  
#define DEFINE_NAMED_RW_OR_COL_BINDER(TG,NAME)				\
  /* Returns a binder to the only row or column type available */	\
  template <class T>  /* Type of the bound expression */		\
  auto NAME(T&& ref,      /*!< Quantity to be bound */			\
	    const int id) /*!< Component to bind    */			\
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
    return bind<Ret>(std::forward<T>(ref),id);				\
  }
  
  // Check that a test Binder is a TEx
  STATIC_ASSERT_IS_TEX(Binder<TensComp<double,1>,Tens<TensKind<TensComp<double,1>>,double>>);
}

#endif
