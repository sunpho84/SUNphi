#ifndef _BIND_HPP
#define _BIND_HPP

/// \file Bind.hpp
///
/// \brief Defines a class which binds a component of a TEx

#include <tens/TensKind.hpp>

namespace SUNphi
{
  /// Class to bind a component of a TEx
  ///
  /// The creator must accepts any type which qualifies as a TEx (TO
  /// BE FIXED)
  template <class B, // Bound type
	    class TG, // Get type
	    class TK=typename std::remove_reference_t<B>::Tk,
	    class TK_TYPES=typename TK::Types>
  class Binder :
    public ConstraintIsTensKind<TK>,
    public ConstraintTupleHasType<TG,TK_TYPES>
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
    
    /// Constructor taking a rvalue
    Binder(B&& ref,int id) : ref(ref),id(id)
    {
    }
    
    // /// Constructor taking lvalue
    // Binder(B& ref,int id) : ref(ref),id(id)
    // {
    // }
    
  };
}

#endif
